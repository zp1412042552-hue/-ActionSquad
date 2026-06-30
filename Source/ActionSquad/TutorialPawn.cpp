#include "TutorialPawn.h"

#include "Camera/CameraComponent.h"
#include "CommandGestureComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInterface.h"
#include "MotionControllerComponent.h"
#include "OculusXRHandComponent.h"
#include "TutorialCommandMarkerActor.h"
#include "TutorialInstructionActor.h"
#include "TutorialDoorActor.h"
#include "TutorialTeamMemberActor.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"

ATutorialPawn::ATutorialPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	BodyCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BodyCollision"));
	BodyCollision->InitCapsuleSize(34.0f, 88.0f);
	BodyCollision->SetCollisionProfileName(TEXT("Pawn"));
	SetRootComponent(BodyCollision);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SceneRoot->SetupAttachment(BodyCollision);

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->SetupAttachment(SceneRoot);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VROrigin);
	Camera->bLockToHmd = true;

	LeftHandTrackingRoot = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHandTrackingRoot"));
	LeftHandTrackingRoot->SetupAttachment(VROrigin);
	LeftHandTrackingRoot->SetTrackingSource(EControllerHand::Left);

	RightHandTrackingRoot = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandTrackingRoot"));
	RightHandTrackingRoot->SetupAttachment(VROrigin);
	RightHandTrackingRoot->SetTrackingSource(EControllerHand::Right);

	LeftHandMesh = CreateDefaultSubobject<UOculusXRHandComponent>(TEXT("LeftHandMesh"));
	LeftHandMesh->SetupAttachment(LeftHandTrackingRoot);
	LeftHandMesh->SkeletonType = EOculusXRHandType::HandLeft;
	LeftHandMesh->MeshType = EOculusXRHandType::HandLeft;
	LeftHandMesh->bInitializePhysics = false;
	LeftHandMesh->bUpdateHandScale = true;
	LeftHandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RightHandMesh = CreateDefaultSubobject<UOculusXRHandComponent>(TEXT("RightHandMesh"));
	RightHandMesh->SetupAttachment(RightHandTrackingRoot);
	RightHandMesh->SkeletonType = EOculusXRHandType::HandRight;
	RightHandMesh->MeshType = EOculusXRHandType::HandRight;
	RightHandMesh->bInitializePhysics = false;
	RightHandMesh->bUpdateHandScale = true;
	RightHandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CommandGesture = CreateDefaultSubobject<UCommandGestureComponent>(TEXT("CommandGesture"));

	TeamMemberClass = ATutorialTeamMemberActor::StaticClass();
	TutorialInstructionClass = ATutorialInstructionActor::StaticClass();
	CommandMarkerClass = ATutorialCommandMarkerActor::StaticClass();

	ConfigureHandVisuals();
}

void ATutorialPawn::BeginPlay()
{
	Super::BeginPlay();
	UHeadMountedDisplayFunctionLibrary::SetSpectatorScreenMode(ESpectatorScreenMode::Disabled);

	if (CommandGesture)
	{
		CommandGesture->OnCommandGestureRecognized.AddDynamic(this, &ATutorialPawn::HandleCommandGestureRecognized);
	}

	SpawnTutorialActors();

	if (APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
	{
		EnableInput(PlayerController);
	}
}

void ATutorialPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateCommandPreview(DeltaSeconds);
}

void ATutorialPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!PlayerInputComponent)
	{
		return;
	}

	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &ATutorialPawn::TestSelectA);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ATutorialPawn::TestSelectB);
	PlayerInputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ATutorialPawn::TestMoveSelectedTeam);
	PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &ATutorialPawn::TestMoveSelectedTeam);
}

void ATutorialPawn::SelectTeam(ESelectedTeamTarget Target)
{
	CurrentSelectedTeam = Target;
	bCommandIssuedSinceSelection = false;
	PreviewHoldSeconds = 0.0f;
	LastPreviewActor.Reset();

	if (TeamA)
	{
		const bool bSelectA = Target == ESelectedTeamTarget::TeamA || Target == ESelectedTeamTarget::All;
		TeamA->SetSelected(bSelectA);
	}

	if (TeamB)
	{
		const bool bSelectB = Target == ESelectedTeamTarget::TeamB || Target == ESelectedTeamTarget::All;
		TeamB->SetSelected(bSelectB);
	}
}

void ATutorialPawn::SpawnTutorialActors()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (TActorIterator<ATutorialTeamMemberActor> It(World); It; ++It)
	{
		ATutorialTeamMemberActor* ExistingMember = *It;
		if (!ExistingMember)
		{
			continue;
		}

		if (ExistingMember->TeamRole == ETeamMemberRole::TeamA && !TeamA)
		{
			TeamA = ExistingMember;
		}
		else if (ExistingMember->TeamRole == ETeamMemberRole::TeamB && !TeamB)
		{
			TeamB = ExistingMember;
		}
	}

	const FRotator SpawnRotation(0.0f, GetActorRotation().Yaw, 0.0f);

	if (!TeamA && TeamMemberClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		TeamA = World->SpawnActor<ATutorialTeamMemberActor>(
			TeamMemberClass,
			GetActorLocation() + SpawnRotation.RotateVector(TeamAOffset),
			SpawnRotation,
			SpawnParams);
		if (TeamA)
		{
			TeamA->InitializeTeamMember(ETeamMemberRole::TeamA);
		}
	}

	if (!TeamB && TeamMemberClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		TeamB = World->SpawnActor<ATutorialTeamMemberActor>(
			TeamMemberClass,
			GetActorLocation() + SpawnRotation.RotateVector(TeamBOffset),
			SpawnRotation,
			SpawnParams);
		if (TeamB)
		{
			TeamB->InitializeTeamMember(ETeamMemberRole::TeamB);
		}
	}

	if (!TutorialInstruction && TutorialInstructionClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		TutorialInstruction = World->SpawnActor<ATutorialInstructionActor>(
			TutorialInstructionClass,
			GetActorLocation() + SpawnRotation.RotateVector(FVector(90.0f, 0.0f, 145.0f)),
			SpawnRotation,
			SpawnParams);
	}

	if (!CommandMarker && CommandMarkerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CommandMarker = World->SpawnActor<ATutorialCommandMarkerActor>(
			CommandMarkerClass,
			GetActorLocation(),
			FRotator::ZeroRotator,
			SpawnParams);
		if (CommandMarker)
		{
			CommandMarker->HideMarker();
		}
	}
}

bool ATutorialPawn::CommandSelectedTeamToPointedLocation()
{
	FHitResult Hit;
	return TraceCommandTarget(Hit) && IssueCommandAtHit(Hit);
}

void ATutorialPawn::HandleCommandGestureRecognized(ECommandGesture Gesture)
{
	switch (Gesture)
	{
	case ECommandGesture::SelectA:
		SelectTeam(ESelectedTeamTarget::TeamA);
		break;
	case ECommandGesture::SelectB:
		SelectTeam(ESelectedTeamTarget::TeamB);
		break;
	case ECommandGesture::Action:
		CommandSelectedTeamToPointedLocation();
		break;
	default:
		break;
	}

	if (TutorialInstruction)
	{
		TutorialInstruction->NotifyGesture(Gesture);
	}
}

void ATutorialPawn::UpdateCommandPreview(float DeltaSeconds)
{
	const ESelectedTeamTarget MarkerTarget = GetMarkerTarget();
	if (MarkerTarget == ESelectedTeamTarget::None || bCommandIssuedSinceSelection)
	{
		PreviewHoldSeconds = 0.0f;
		LastPreviewActor.Reset();
		if (CommandMarker)
		{
			CommandMarker->HideMarker();
		}
		return;
	}

	FHitResult Hit;
	if (!TraceCommandTarget(Hit))
	{
		PreviewHoldSeconds = 0.0f;
		LastPreviewActor.Reset();
		if (CommandMarker)
		{
			CommandMarker->HideMarker();
		}
		return;
	}

	const bool bSameActor = LastPreviewActor.Get() == Hit.GetActor();
	const bool bSameLocation = FVector::DistSquared(LastPreviewLocation, Hit.ImpactPoint) <= FMath::Square(CommandStableRadius);
	if (bSameActor && bSameLocation)
	{
		PreviewHoldSeconds += FMath::Max(0.0f, DeltaSeconds);
	}
	else
	{
		PreviewHoldSeconds = 0.0f;
		LastPreviewLocation = Hit.ImpactPoint;
		LastPreviewActor = Hit.GetActor();
	}

	if (CommandMarker)
	{
		CommandMarker->ShowMarker(
			MarkerTarget,
			Hit.ImpactPoint,
			Hit.ImpactNormal,
			CommandHoldSeconds > KINDA_SMALL_NUMBER ? PreviewHoldSeconds / CommandHoldSeconds : 1.0f);
	}

	if (PreviewHoldSeconds >= CommandHoldSeconds)
	{
		if (IssueCommandAtHit(Hit))
		{
			bCommandIssuedSinceSelection = true;
			if (TutorialInstruction)
			{
				TutorialInstruction->NotifyGesture(ECommandGesture::Action);
			}
			PreviewHoldSeconds = 0.0f;
			LastPreviewActor.Reset();
		}
	}
}

bool ATutorialPawn::TraceCommandTarget(FHitResult& OutHit) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	USceneComponent* TraceSource = RightHandTrackingRoot ? Cast<USceneComponent>(RightHandTrackingRoot) : nullptr;
	if (!TraceSource || !TraceSource->IsRegistered())
	{
		TraceSource = Camera;
	}

	if (!TraceSource)
	{
		return false;
	}

	const FVector Start = TraceSource->GetComponentLocation();
	const FVector End = Start + TraceSource->GetForwardVector() * CommandTraceDistance;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ActionSquadCommandTrace), false, this);
	return World->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, QueryParams);
}

bool ATutorialPawn::IssueCommandAtHit(const FHitResult& Hit)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	ATutorialDoorActor* HitDoor = Cast<ATutorialDoorActor>(Hit.GetActor());
	bool bIssuedCommand = false;
	if ((CurrentSelectedTeam == ESelectedTeamTarget::TeamA || CurrentSelectedTeam == ESelectedTeamTarget::All) && TeamA)
	{
		if (HitDoor)
		{
			TeamA->BreachDoor(HitDoor);
		}
		else
		{
			TeamA->MoveToCommandLocation(Hit.ImpactPoint);
		}
		bIssuedCommand = true;
	}

	if ((CurrentSelectedTeam == ESelectedTeamTarget::TeamB || CurrentSelectedTeam == ESelectedTeamTarget::All) && TeamB)
	{
		if (HitDoor)
		{
			TeamB->BreachDoor(HitDoor);
		}
		else
		{
			TeamB->MoveToCommandLocation(Hit.ImpactPoint);
		}
		bIssuedCommand = true;
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (bIssuedCommand)
	{
		DrawDebugSphere(World, Hit.ImpactPoint, 12.0f, 12, FColor::Cyan, false, 1.5f, 0, 2.0f);
	}
#endif

	return bIssuedCommand;
}

ESelectedTeamTarget ATutorialPawn::GetMarkerTarget() const
{
	if (CurrentSelectedTeam == ESelectedTeamTarget::TeamA || CurrentSelectedTeam == ESelectedTeamTarget::TeamB)
	{
		return CurrentSelectedTeam;
	}

	return ESelectedTeamTarget::None;
}

void ATutorialPawn::TestSelectA()
{
	if (CommandGesture)
	{
		CommandGesture->ForceRecognizeGesture(ECommandGesture::SelectA);
	}
}

void ATutorialPawn::TestSelectB()
{
	if (CommandGesture)
	{
		CommandGesture->ForceRecognizeGesture(ECommandGesture::SelectB);
	}
}

void ATutorialPawn::TestMoveSelectedTeam()
{
	if (CommandSelectedTeamToPointedLocation() && TutorialInstruction)
	{
		TutorialInstruction->NotifyGesture(ECommandGesture::Action);
	}
}

void ATutorialPawn::ConfigureHandVisuals()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HandMaterial(
		TEXT("/Game/HandGameplay/Hands/Models/HandMat.HandMat"));

	if (HandMaterial.Succeeded())
	{
		if (LeftHandMesh)
		{
			LeftHandMesh->MaterialOverride = HandMaterial.Object;
			LeftHandMesh->SetMaterial(0, HandMaterial.Object);
		}
		if (RightHandMesh)
		{
			RightHandMesh->MaterialOverride = HandMaterial.Object;
			RightHandMesh->SetMaterial(0, HandMaterial.Object);
		}
	}
}
