#include "CommandGestureComponent.h"

#include "Camera/PlayerCameraManager.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "GesturePoseProfileSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "OculusXRInputFunctionLibrary.h"

UCommandGestureComponent::UCommandGestureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCommandGestureComponent::BeginPlay()
{
	Super::BeginPlay();
	LoadRecordedGestureProfile();
}

void UCommandGestureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bEnableMetaHandPolling)
	{
		return;
	}

	FFingerExtensionPose PolledPose;
	FHandPose PolledHandPose;
	if (PollMetaHandPose(PolledHandPose, &PolledPose))
	{
		LastFingerPose = PolledPose;

		FVector HandLocation = FVector::ZeroVector;
		bHandInsideRecognitionZone =
			!bUseRecognitionZone ||
			(GetRecognizerHandLocation(HandLocation) && IsHandInsideRecognitionZone(HandLocation));

		if (!bHandInsideRecognitionZone)
		{
			LastRecordedHandPoseConfidence = 0.0f;
			LastRecordedHandPoseError = 0.0f;
			LastRecordedFingerPoseConfidence = 0.0f;
			UpdateCandidate(ECommandGesture::None, DeltaTime);
			return;
		}

		float BestConfidence = 0.0f;
		float BestError = 0.0f;
		float BestFingerConfidence = 0.0f;
		ECommandGesture Candidate = ClassifyHandPose(PolledHandPose, PolledPose, &BestConfidence, &BestError, &BestFingerConfidence);
		LastRecordedHandPoseConfidence = BestConfidence;
		LastRecordedHandPoseError = BestError;
		LastRecordedFingerPoseConfidence = BestFingerConfidence;
		DrawRecognitionZone(Candidate);
		UpdateCandidate(Candidate, DeltaTime);
	}
	else
	{
		bHandInsideRecognitionZone = false;
		UpdateCandidate(ECommandGesture::None, DeltaTime);
	}
}

void UCommandGestureComponent::SubmitFingerExtensionPose(const FFingerExtensionPose& Pose)
{
	LastFingerPose.Thumb = Clamp01(Pose.Thumb);
	LastFingerPose.Index = Clamp01(Pose.Index);
	LastFingerPose.Middle = Clamp01(Pose.Middle);
	LastFingerPose.Ring = Clamp01(Pose.Ring);
	LastFingerPose.Pinky = Clamp01(Pose.Pinky);
}

void UCommandGestureComponent::ForceRecognizeGesture(ECommandGesture Gesture)
{
	if (Gesture == ECommandGesture::None)
	{
		return;
	}

	StableGesture = Gesture;
	StableGestureSeconds = RequiredHoldSeconds;
	OnCommandGestureRecognized.Broadcast(Gesture);
}

bool UCommandGestureComponent::GetCurrentHandPose(FHandPose& OutHandPose, FFingerExtensionPose* OutFingerPose) const
{
	return PollMetaHandPose(OutHandPose, OutFingerPose);
}

void UCommandGestureComponent::LoadRecordedGestureProfile()
{
	if (!UGameplayStatics::DoesSaveGameExist(UGesturePoseProfileSaveGame::SlotName, UGesturePoseProfileSaveGame::UserIndex))
	{
		return;
	}

	UGesturePoseProfileSaveGame* Profile = Cast<UGesturePoseProfileSaveGame>(
		UGameplayStatics::LoadGameFromSlot(UGesturePoseProfileSaveGame::SlotName, UGesturePoseProfileSaveGame::UserIndex));
	if (!Profile)
	{
		return;
	}

	bHasRecordedSelectAHandPose = false;
	bHasRecordedSelectBHandPose = false;
	bHasRecordedSelectAFingerPose = false;
	bHasRecordedSelectBFingerPose = false;

	if (!Profile->SelectAEncodedPose.IsEmpty())
	{
		RecordedSelectAHandPose.PoseName = TEXT("SelectA");
		RecordedSelectAHandPose.CustomEncodedPose = Profile->SelectAEncodedPose;
		bHasRecordedSelectAHandPose = RecordedSelectAHandPose.Decode();
	}
	if (Profile->bHasSelectA)
	{
		RecordedSelectAFingerPose = Profile->SelectAPose;
		bHasRecordedSelectAFingerPose = true;
	}

	if (!Profile->SelectBEncodedPose.IsEmpty())
	{
		RecordedSelectBHandPose.PoseName = TEXT("SelectB");
		RecordedSelectBHandPose.CustomEncodedPose = Profile->SelectBEncodedPose;
		bHasRecordedSelectBHandPose = RecordedSelectBHandPose.Decode();
	}
	if (Profile->bHasSelectB)
	{
		RecordedSelectBFingerPose = Profile->SelectBPose;
		bHasRecordedSelectBFingerPose = true;
	}
}

void UCommandGestureComponent::UpdateCandidate(ECommandGesture Candidate, float DeltaTime)
{
	if (Candidate == ECommandGesture::None)
	{
		StableGesture = ECommandGesture::None;
		StableGestureSeconds = 0.0f;
		return;
	}

	if (StableGesture != Candidate)
	{
		StableGesture = Candidate;
		StableGestureSeconds = 0.0f;
		return;
	}

	StableGestureSeconds += FMath::Max(0.0f, DeltaTime);
	if (StableGestureSeconds >= RequiredHoldSeconds)
	{
		StableGestureSeconds = 0.0f;
		OnCommandGestureRecognized.Broadcast(Candidate);
	}
}

ECommandGesture UCommandGestureComponent::ClassifyHandPose(const FHandPose& Pose, const FFingerExtensionPose& FingerPose, float* OutBestConfidence, float* OutBestError, float* OutBestFingerConfidence) const
{
	float BestConfidence = 0.0f;
	float SecondBestConfidence = 0.0f;
	float BestError = TNumericLimits<float>::Max();
	float BestFingerConfidence = 0.0f;
	bool bBestPassesFloor = false;
	ECommandGesture BestGesture = ECommandGesture::None;

	auto ConsiderGesture = [&](ECommandGesture Gesture, const FHandPose* RecordedHandPose, const FFingerExtensionPose* RecordedFingerPose)
	{
		float HandConfidence = 0.0f;
		float RawError = 0.0f;
		if (RecordedHandPose)
		{
			HandConfidence = RecordedHandPose->ComputeConfidence(Pose, &RawError);
		}

		float FingerConfidence = 0.0f;
		if (RecordedFingerPose)
		{
			FingerConfidence = ComputeFingerPoseConfidence(*RecordedFingerPose, FingerPose);
		}

		const float Confidence = FMath::Max(HandConfidence, FingerConfidence);
		const bool bPassesFloor =
			HandConfidence >= RecordedHandPoseConfidenceFloor ||
			FingerConfidence >= RecordedFingerPoseConfidenceFloor;

		if (Confidence > BestConfidence)
		{
			SecondBestConfidence = BestConfidence;
			BestConfidence = Confidence;
			BestError = RawError;
			BestFingerConfidence = FingerConfidence;
			bBestPassesFloor = bPassesFloor;
			BestGesture = Gesture;
		}
		else if (Confidence > SecondBestConfidence)
		{
			SecondBestConfidence = Confidence;
		}
	};

	if (bHasRecordedSelectAHandPose || bHasRecordedSelectAFingerPose)
	{
		ConsiderGesture(
			ECommandGesture::SelectA,
			bHasRecordedSelectAHandPose ? &RecordedSelectAHandPose : nullptr,
			bHasRecordedSelectAFingerPose ? &RecordedSelectAFingerPose : nullptr);
	}

	if (bHasRecordedSelectBHandPose || bHasRecordedSelectBFingerPose)
	{
		ConsiderGesture(
			ECommandGesture::SelectB,
			bHasRecordedSelectBHandPose ? &RecordedSelectBHandPose : nullptr,
			bHasRecordedSelectBFingerPose ? &RecordedSelectBFingerPose : nullptr);
	}

	if (OutBestConfidence)
	{
		*OutBestConfidence = BestConfidence;
	}
	if (OutBestError)
	{
		*OutBestError = BestError == TNumericLimits<float>::Max() ? 0.0f : BestError;
	}
	if (OutBestFingerConfidence)
	{
		*OutBestFingerConfidence = BestFingerConfidence;
	}

	if (!bBestPassesFloor)
	{
		return ECommandGesture::None;
	}

	if (BestConfidence - SecondBestConfidence < RecordedGestureConfidenceMargin)
	{
		return ECommandGesture::None;
	}

	return BestGesture;
}

bool UCommandGestureComponent::PollMetaHandPose(FHandPose& OutHandPose, FFingerExtensionPose* OutFingerPose) const
{
	if (!UOculusXRInputFunctionLibrary::IsHandTrackingEnabled())
	{
		return false;
	}

	const EOculusXRHandType Hand = EOculusXRHandType::HandRight;
	if (UOculusXRInputFunctionLibrary::GetTrackingConfidence(Hand) == EOculusXRTrackingConfidence::Low)
	{
		return false;
	}

	OutHandPose.UpdatePose(Hand, GetRecognizerWristRotator());

	auto FingerExtension = [Hand](EOculusXRBone A, EOculusXRBone B, EOculusXRBone C)
	{
		const FQuat QA = UOculusXRInputFunctionLibrary::GetBoneRotation(Hand, A);
		const FQuat QB = UOculusXRInputFunctionLibrary::GetBoneRotation(Hand, B);
		const FQuat QC = UOculusXRInputFunctionLibrary::GetBoneRotation(Hand, C);

		const float AB = FMath::RadiansToDegrees(QA.AngularDistance(QB));
		const float BC = FMath::RadiansToDegrees(QB.AngularDistance(QC));
		const float CurlDegrees = AB + BC;
		return FMath::Clamp(1.0f - (CurlDegrees / 135.0f), 0.0f, 1.0f);
	};

	if (OutFingerPose)
	{
		OutFingerPose->Thumb = FingerExtension(EOculusXRBone::Thumb_1, EOculusXRBone::Thumb_2, EOculusXRBone::Thumb_3);
		OutFingerPose->Index = FingerExtension(EOculusXRBone::Index_1, EOculusXRBone::Index_2, EOculusXRBone::Index_3);
		OutFingerPose->Middle = FingerExtension(EOculusXRBone::Middle_1, EOculusXRBone::Middle_2, EOculusXRBone::Middle_3);
		OutFingerPose->Ring = FingerExtension(EOculusXRBone::Ring_1, EOculusXRBone::Ring_2, EOculusXRBone::Ring_3);
		OutFingerPose->Pinky = FingerExtension(EOculusXRBone::Pinky_1, EOculusXRBone::Pinky_2, EOculusXRBone::Pinky_3);
	}

	return true;
}

bool UCommandGestureComponent::GetRecognizerHandLocation(FVector& OutHandLocation) const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	TArray<UMotionControllerComponent*> MotionControllers;
	Owner->GetComponents(MotionControllers);
	for (const UMotionControllerComponent* MotionController : MotionControllers)
	{
		if (MotionController && MotionController->GetName().Contains(TEXT("RightHandTrackingRoot")))
		{
			OutHandLocation = MotionController->GetComponentLocation();
			return true;
		}
	}

	return false;
}

bool UCommandGestureComponent::IsHandInsideRecognitionZone(const FVector& HandLocation) const
{
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	const APlayerCameraManager* CameraManager = PlayerController ? PlayerController->PlayerCameraManager : nullptr;
	if (!CameraManager)
	{
		return false;
	}

	const FTransform CameraTransform(CameraManager->GetCameraRotation(), CameraManager->GetCameraLocation());
	const FVector LocalHandLocation = CameraTransform.InverseTransformPosition(HandLocation);
	const FVector Min = RecognitionZoneCenter - RecognitionZoneExtent;
	const FVector Max = RecognitionZoneCenter + RecognitionZoneExtent;

	return
		LocalHandLocation.X >= Min.X && LocalHandLocation.X <= Max.X &&
		LocalHandLocation.Y >= Min.Y && LocalHandLocation.Y <= Max.Y &&
		LocalHandLocation.Z >= Min.Z && LocalHandLocation.Z <= Max.Z;
}

void UCommandGestureComponent::DrawRecognitionZone(ECommandGesture Candidate) const
{
	if (!bDrawRecognitionZone || !bUseRecognitionZone)
	{
		return;
	}

	UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	const APlayerCameraManager* CameraManager = PlayerController ? PlayerController->PlayerCameraManager : nullptr;
	if (!World || !CameraManager)
	{
		return;
	}

	const FTransform CameraTransform(CameraManager->GetCameraRotation(), CameraManager->GetCameraLocation());
	const FVector BoxCenter = CameraTransform.TransformPosition(RecognitionZoneCenter);
	const FQuat BoxRotation = CameraTransform.GetRotation();
	const FColor BoxColor = Candidate == ECommandGesture::None ? FColor(0, 120, 255) : FColor::Cyan;
	DrawDebugBox(World, BoxCenter, RecognitionZoneExtent, BoxRotation, BoxColor, false, 0.0f, 0, 1.8f);
}

FRotator UCommandGestureComponent::GetRecognizerWristRotator() const
{
	FQuat ComponentQuat = FQuat::Identity;

	if (const AActor* Owner = GetOwner())
	{
		TArray<UMotionControllerComponent*> MotionControllers;
		Owner->GetComponents(MotionControllers);
		for (const UMotionControllerComponent* MotionController : MotionControllers)
		{
			if (MotionController && MotionController->GetName().Contains(TEXT("RightHandTrackingRoot")))
			{
				ComponentQuat = MotionController->GetComponentQuat();
				break;
			}
		}

		if (ComponentQuat.IsIdentity() && MotionControllers.Num() > 0 && MotionControllers[0])
		{
			ComponentQuat = MotionControllers[0]->GetComponentQuat();
		}

		if (ComponentQuat.IsIdentity() && Owner->GetRootComponent())
		{
			ComponentQuat = Owner->GetRootComponent()->GetComponentQuat();
		}
	}

	FRotator ComponentRotator = ComponentQuat.Rotator();
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		return ComponentRotator;
	}

	const FRotator ComponentRotationToCamera =
		PlayerController->PlayerCameraManager->GetTransform().InverseTransformRotation(ComponentQuat).Rotator();
	ComponentRotator.Yaw = ComponentRotationToCamera.Yaw;
	return ComponentRotator;
}

float UCommandGestureComponent::Clamp01(float Value)
{
	return FMath::Clamp(Value, 0.0f, 1.0f);
}

float UCommandGestureComponent::ComputeFingerPoseConfidence(const FFingerExtensionPose& ReferencePose, const FFingerExtensionPose& Pose)
{
	const float AverageAbsoluteError =
		(FMath::Abs(ReferencePose.Thumb - Pose.Thumb) +
			FMath::Abs(ReferencePose.Index - Pose.Index) +
			FMath::Abs(ReferencePose.Middle - Pose.Middle) +
			FMath::Abs(ReferencePose.Ring - Pose.Ring) +
			FMath::Abs(ReferencePose.Pinky - Pose.Pinky)) / 5.0f;

	return FMath::Clamp(1.0f - AverageAbsoluteError, 0.0f, 1.0f);
}
