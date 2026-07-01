#include "TutorialInstructionActor.h"

#include "Camera/PlayerCameraManager.h"
#include "Components/WidgetComponent.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TutorialCompletionZoneActor.h"
#include "TutorialCommandWidget.h"
#include "TutorialDoorActor.h"
#include "TutorialFloorMarkerActor.h"
#include "TutorialGestureDisplayActor.h"
#include "TutorialPawn.h"
#include "TutorialTeamMemberActor.h"

ATutorialInstructionActor::ATutorialInstructionActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TutorialWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TutorialWidget"));
	TutorialWidgetComponent->SetupAttachment(SceneRoot);
	TutorialWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	TutorialWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TutorialWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TutorialWidgetComponent->SetGenerateOverlapEvents(false);
	TutorialWidgetComponent->SetTwoSided(false);
	TutorialWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	TutorialWidgetComponent->SetBlendMode(EWidgetBlendMode::Transparent);

	BuildDefaultSteps();
	ApplyScreenLayout();
}

void ATutorialInstructionActor::BeginPlay()
{
	Super::BeginPlay();
	RemoveDuplicateInstructionActors();
	if (IsActorBeingDestroyed())
	{
		return;
	}
	StartTutorial();
}

void ATutorialInstructionActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateObjectiveProgress();

	if (EnsureCamera())
	{
		UpdateTransform(DeltaSeconds);
	}
}

void ATutorialInstructionActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyScreenLayout();
}

void ATutorialInstructionActor::StartTutorial()
{
	BuildDefaultSteps();
	InitializeWidget();
	ResolveGestureDisplayActor();
	ResolveTutorialTargets();
	SetCurrentStep(0);
}

void ATutorialInstructionActor::NotifyGesture(ECommandGesture Gesture)
{
	if (!Steps.IsValidIndex(CurrentStepIndex))
	{
		return;
	}

	const FCommandTutorialStep& Step = Steps[CurrentStepIndex];
	if (Step.RequiredGesture == Gesture)
	{
		if (CurrentStepIndex == 1 || CurrentStepIndex == 2 || CurrentStepIndex == 3)
		{
			if (CachedWidget)
			{
				CachedWidget->SetFooterText(FText::FromString(TEXT("很好，继续指向目标，让队友完成动作。")));
			}
			return;
		}

		SetCurrentStep(FMath::Min(CurrentStepIndex + 1, Steps.Num() - 1));
		return;
	}

	if (CachedWidget)
	{
		CachedWidget->SetFooterText(FText::FromString(TEXT("已识别手势，队友状态已更新。")));
	}
}

void ATutorialInstructionActor::NotifyPlayerFiredWeapon()
{
	if (IsStep(0))
	{
		SetCurrentStep(1);
	}
}

void ATutorialInstructionActor::NotifyDoorBreached(ATutorialDoorActor* Door)
{
	if (!IsStep(3))
	{
		return;
	}

	if (!BreachDoor || BreachDoor == Door)
	{
		SetCurrentStep(4);
	}
}

void ATutorialInstructionActor::NotifyPlayerEnteredCompletionZone(ATutorialCompletionZoneActor* Zone)
{
	if (!IsStep(5))
	{
		return;
	}

	if (Zone && Zone->bOnlyCompleteAfterDoorBreached && BreachDoor && BreachDoor->DoorState != ETutorialDoorState::Breached)
	{
		return;
	}

	if (!CompletionZone || CompletionZone == Zone)
	{
		CompleteTutorial();
	}
}

void ATutorialInstructionActor::SetCurrentStep(int32 StepIndex)
{
	CurrentStepIndex = Steps.IsValidIndex(StepIndex) ? StepIndex : INDEX_NONE;
	bHasStepStartPlayerLocation = false;
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		StepStartPlayerLocation = PlayerPawn->GetActorLocation();
		bHasStepStartPlayerLocation = true;
	}
	ApplyCurrentStepToWidget();
}

void ATutorialInstructionActor::BuildDefaultSteps()
{
	Steps.Reset();

	FCommandTutorialStep Fire;
	Fire.Title = FText::FromString(TEXT("1/6 开火"));
	Fire.Highlight = FText::FromString(TEXT("抬起双手，轻碰开火"));
	Fire.Body = FText::FromString(TEXT("左手持枪，右手靠近左手。两只手轻轻贴近时，武器会发射一枪。"));
	Fire.FooterHint = FText::FromString(TEXT("开出第一枪后，继续学习队友指挥。"));
	Fire.RequiredGesture = ECommandGesture::None;
	Fire.GestureDisplayIndex = INDEX_NONE;
	Steps.Add(Fire);

	FCommandTutorialStep MoveA;
	MoveA.Title = FText::FromString(TEXT("2/6 队友 A 到位"));
	MoveA.Highlight = FText::FromString(TEXT("伸出 1 根手指，指向 A 标识"));
	MoveA.Body = FText::FromString(TEXT("选择队友 A，然后把指向线落在门口地面的 A 标识上。队友 A 到达站位后，会自动进入下一步。"));
	MoveA.FooterHint = FText::FromString(TEXT("让指向线保持在 A 标识附近，直到队友开始移动。"));
	MoveA.RequiredGesture = ECommandGesture::SelectA;
	MoveA.GestureDisplayIndex = 0;
	Steps.Add(MoveA);

	FCommandTutorialStep MoveB;
	MoveB.Title = FText::FromString(TEXT("3/6 队友 B 到位"));
	MoveB.Highlight = FText::FromString(TEXT("伸出 2 根手指，指向 B 标识"));
	MoveB.Body = FText::FromString(TEXT("选择队友 B，然后把指向线落在门口地面的 B 标识上。队友 B 到达站位后，会自动进入下一步。"));
	MoveB.FooterHint = FText::FromString(TEXT("让指向线保持在 B 标识附近，直到队友开始移动。"));
	MoveB.RequiredGesture = ECommandGesture::SelectB;
	MoveB.GestureDisplayIndex = 1;
	Steps.Add(MoveB);

	FCommandTutorialStep BreachA;
	BreachA.Title = FText::FromString(TEXT("4/6 破门"));
	BreachA.Highlight = FText::FromString(TEXT("选择队友 A，指向门"));
	BreachA.Body = FText::FromString(TEXT("再次选择队友 A，把指向线落在门上。队友 A 会靠近门口，并执行破门动作。"));
	BreachA.FooterHint = FText::FromString(TEXT("门被踹开后，先学习移动，再穿过门。"));
	BreachA.RequiredGesture = ECommandGesture::SelectA;
	BreachA.GestureDisplayIndex = 2;
	Steps.Add(BreachA);

	FCommandTutorialStep MovePlayer;
	MovePlayer.Title = FText::FromString(TEXT("5/6 前进与后退"));
	MovePlayer.Highlight = FText::FromString(TEXT("枪口下压前进，枪口上抬后退"));
	MovePlayer.Body = FText::FromString(TEXT("左手握住枪。把枪口向下压，会向当前视线方向前进；把枪口向上抬，会向后退。需要转身时，请在现实中原地转身。"));
	MovePlayer.FooterHint = FText::FromString(TEXT("现在向前移动一小段距离，教程会自动进入最后一步。"));
	MovePlayer.RequiredGesture = ECommandGesture::None;
	MovePlayer.GestureDisplayIndex = INDEX_NONE;
	Steps.Add(MovePlayer);

	FCommandTutorialStep EnterRoom;
	EnterRoom.Title = FText::FromString(TEXT("6/6 进入房间"));
	EnterRoom.Highlight = FText::FromString(TEXT("穿过门"));
	EnterRoom.Body = FText::FromString(TEXT("门已经打开。现在从门口通过，进入另一间房，完成本段教程。"));
	EnterRoom.FooterHint = FText::FromString(TEXT("进入门后的完成区域后，教程会自动结束。"));
	EnterRoom.RequiredGesture = ECommandGesture::None;
	EnterRoom.GestureDisplayIndex = INDEX_NONE;
	Steps.Add(EnterRoom);
}

void ATutorialInstructionActor::RemoveDuplicateInstructionActors()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (TActorIterator<ATutorialInstructionActor> It(World); It; ++It)
	{
		ATutorialInstructionActor* Other = *It;
		if (!Other || Other == this)
		{
			continue;
		}

		if (Other->GetUniqueID() < GetUniqueID())
		{
			Destroy();
			return;
		}
	}

	for (TActorIterator<ATutorialInstructionActor> It(World); It; ++It)
	{
		ATutorialInstructionActor* Other = *It;
		if (Other && Other != this)
		{
			Other->Destroy();
		}
	}
}

void ATutorialInstructionActor::InitializeWidget()
{
	if (!TutorialWidgetComponent)
	{
		return;
	}

	ApplyScreenLayout();
	TutorialWidgetComponent->SetWidgetClass(UTutorialCommandWidget::StaticClass());
	TutorialWidgetComponent->InitWidget();
	CachedWidget = Cast<UTutorialCommandWidget>(TutorialWidgetComponent->GetWidget());
}

void ATutorialInstructionActor::ApplyScreenLayout()
{
	if (TutorialWidgetComponent)
	{
		TutorialWidgetComponent->SetDrawSize(WidgetDrawSize);
	}

	SetActorScale3D(FVector(FMath::Max(0.01f, ScreenScale)));
}

void ATutorialInstructionActor::ApplyCurrentStepToWidget()
{
	if (!CachedWidget || !Steps.IsValidIndex(CurrentStepIndex))
	{
		return;
	}

	const FCommandTutorialStep& Step = Steps[CurrentStepIndex];
	CachedWidget->SetTitleText(Step.Title);
	CachedWidget->SetHighlightText(Step.Highlight);
	CachedWidget->SetBodyText(Step.Body);
	CachedWidget->SetFooterText(Step.FooterHint);
	CachedWidget->SetStepIndicator(CurrentStepIndex + 1, Steps.Num());

	ResolveGestureDisplayActor();
	if (GestureDisplayActor)
	{
		GestureDisplayActor->SetActorHiddenInGame(false);
		GestureDisplayActor->PlayGestureDemo(Step.GestureDisplayIndex);
		UpdateGestureDisplayTransform();
	}
}

void ATutorialInstructionActor::ResolveGestureDisplayActor()
{
	if (GestureDisplayActor || !GetWorld())
	{
		return;
	}

	if (!bAutoSpawnGestureDisplayActor)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GestureDisplayActor = GetWorld()->SpawnActor<ATutorialGestureDisplayActor>(
		ATutorialGestureDisplayActor::StaticClass(),
		GetActorTransform(),
		SpawnParams);
}

void ATutorialInstructionActor::ResolveTutorialTargets()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (TActorIterator<ATutorialFloorMarkerActor> It(World); It; ++It)
	{
		ATutorialFloorMarkerActor* Marker = *It;
		if (!Marker)
		{
			continue;
		}

		if (Marker->MarkerTarget == ESelectedTeamTarget::TeamA && !TeamAMarker)
		{
			TeamAMarker = Marker;
		}
		else if (Marker->MarkerTarget == ESelectedTeamTarget::TeamB && !TeamBMarker)
		{
			TeamBMarker = Marker;
		}
	}

	if (!BreachDoor)
	{
		for (TActorIterator<ATutorialDoorActor> It(World); It; ++It)
		{
			BreachDoor = *It;
			break;
		}
	}

	if (!CompletionZone)
	{
		for (TActorIterator<ATutorialCompletionZoneActor> It(World); It; ++It)
		{
			CompletionZone = *It;
			break;
		}
	}
}

void ATutorialInstructionActor::UpdateObjectiveProgress()
{
	if (!Steps.IsValidIndex(CurrentStepIndex))
	{
		return;
	}

	ResolveTutorialTargets();

	ATutorialPawn* TutorialPawn = Cast<ATutorialPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!TutorialPawn)
	{
		return;
	}

	if (IsStep(1) && TeamAMarker && TeamAMarker->IsTeamMemberAtMarker(TutorialPawn->TeamA))
	{
		SetCurrentStep(2);
		return;
	}

	if (IsStep(2) && TeamBMarker && TeamBMarker->IsTeamMemberAtMarker(TutorialPawn->TeamB))
	{
		SetCurrentStep(3);
		return;
	}

	if (IsStep(3) && BreachDoor && BreachDoor->DoorState == ETutorialDoorState::Breached)
	{
		SetCurrentStep(4);
		return;
	}

	if (IsStep(4) && bHasStepStartPlayerLocation)
	{
		const float RequiredDistance = FMath::Max(1.0f, LocomotionTutorialForwardDistance);
		if (FVector::DistSquared2D(TutorialPawn->GetActorLocation(), StepStartPlayerLocation) >= FMath::Square(RequiredDistance))
		{
			SetCurrentStep(5);
		}
	}
}

void ATutorialInstructionActor::CompleteTutorial()
{
	if (CachedWidget)
	{
		CachedWidget->SetTitleText(FText::FromString(TEXT("教程完成")));
		CachedWidget->SetHighlightText(FText::FromString(TEXT("队友指挥已完成")));
		CachedWidget->SetBodyText(FText::FromString(TEXT("你已经完成开火、站位、指挥、破门和移动流程。")));
		CachedWidget->SetFooterText(FText::FromString(TEXT("可以继续进入下一段教程。")));
		CachedWidget->SetStepIndicator(Steps.Num(), Steps.Num());
	}

	if (GestureDisplayActor)
	{
		GestureDisplayActor->SetActorHiddenInGame(true);
	}

	CurrentStepIndex = INDEX_NONE;
}

void ATutorialInstructionActor::UpdateTransform(float DeltaSeconds)
{
	if (!PlayerCameraManager)
	{
		return;
	}

	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
	const FRotator CameraRotation = PlayerCameraManager->GetCameraRotation();
	const FVector TargetLocation = CameraLocation + CameraRotation.Vector() * FollowDistance + FVector(0.0f, 0.0f, VerticalOffset);

	if (!bHasInitialTransformLock)
	{
		const FRotator InitialRotation = UKismetMathLibrary::FindLookAtRotation(TargetLocation, CameraLocation);
		SetActorLocationAndRotation(TargetLocation, FRotator(InitialRotation.Pitch, InitialRotation.Yaw, 0.0f));
		bHasInitialTransformLock = true;
		UpdateGestureDisplayTransform();
		return;
	}

	FVector NewLocation = GetActorLocation();
	if (FVector::DistSquared(NewLocation, TargetLocation) > FMath::Square(0.5f))
	{
		NewLocation = FMath::VInterpTo(NewLocation, TargetLocation, DeltaSeconds, FMath::Max(LocationLerpSpeed, KINDA_SMALL_NUMBER));
	}

	const FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(NewLocation, CameraLocation);
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaSeconds, FMath::Max(RotationLerpSpeed, KINDA_SMALL_NUMBER));
	NewRotation.Roll = 0.0f;

	SetActorLocationAndRotation(NewLocation, NewRotation);
	UpdateGestureDisplayTransform();
}

void ATutorialInstructionActor::UpdateGestureDisplayTransform()
{
	if (!GestureDisplayActor)
	{
		return;
	}

	const FVector GestureLocation = GetActorLocation() + GetActorQuat().RotateVector(GestureDisplayActor->WidgetOffset);
	const FQuat GestureRotation =
		GetActorQuat()
		* FQuat(FVector::UpVector, PI)
		* FQuat(FVector::UpVector, FMath::DegreesToRadians(GestureDisplayActor->ModelForwardCorrectionYawDegrees));

	GestureDisplayActor->SetTargetDisplayWidth(WidgetDrawSize.X * FMath::Max(0.01f, ScreenScale) * GestureDisplayActor->TargetWidthFractionOfScreen);
	GestureDisplayActor->SetActorLocationAndRotation(GestureLocation, GestureRotation.Rotator());
}

bool ATutorialInstructionActor::EnsureCamera()
{
	if (!PlayerCameraManager)
	{
		PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	}

	return PlayerCameraManager != nullptr;
}

bool ATutorialInstructionActor::IsStep(int32 StepIndex) const
{
	return CurrentStepIndex == StepIndex && Steps.IsValidIndex(CurrentStepIndex);
}
