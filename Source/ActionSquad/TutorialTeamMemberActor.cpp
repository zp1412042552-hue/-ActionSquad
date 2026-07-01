#include "TutorialTeamMemberActor.h"

#include "AIController.h"
#include "Animation/AnimSequence.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "TeamNameplateWidget.h"
#include "TutorialBallisticEffectActor.h"
#include "TutorialBulletMarkActor.h"
#include "TutorialDoorActor.h"
#include "TutorialWeaponActor.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ATutorialTeamMemberActor::ATutorialTeamMemberActor()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	GetCapsuleComponent()->InitCapsuleSize(34.0f, 92.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	SoldierMesh = GetMesh();
	SoldierMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	SoldierMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -92.0f));
	SoldierMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = MovementSpeed;
		Movement->MaxStepHeight = StepUpHeight;
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.0f, 420.0f, 0.0f);
	}
	bUseControllerRotationYaw = false;

	NameplateWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameplateWidget"));
	NameplateWidget->SetupAttachment(GetCapsuleComponent());
	NameplateWidget->SetRelativeLocation(FVector(0.0f, 0.0f, NameplateHeight));
	NameplateWidget->SetWidgetSpace(EWidgetSpace::World);
	NameplateWidget->SetWidgetClass(UTeamNameplateWidget::StaticClass());
	NameplateWidget->SetDrawSize(FVector2D(220.0f, 110.0f));
	NameplateWidget->SetPivot(FVector2D(0.5f, 0.5f));
	NameplateWidget->SetTwoSided(true);
	NameplateWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NameplateWidget->SetGenerateOverlapEvents(false);
	NameplateWidget->SetRelativeScale3D(FVector(0.18f));

	WeaponActorClass = ATutorialWeaponActor::StaticClass();
	MuzzleFlashEffectClass = ATutorialBallisticEffectActor::StaticClass();
	BulletTracerEffectClass = ATutorialBallisticEffectActor::StaticClass();
	ImpactEffectClass = ATutorialBallisticEffectActor::StaticClass();
	BulletMarkClass = ATutorialBulletMarkActor::StaticClass();

	LoadDefaultAssets();
}

void ATutorialTeamMemberActor::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = FMath::Max(1.0f, MaxHealth);
	bDead = false;
	SnapToGround();
	RefreshNameplate();
	SpawnAndAttachWeapon();
	SetSelected(bSelected);
	PlayTeamAnimation(bSelected ? ETeamMemberAnimState::AlertIdle : ETeamMemberAnimState::RelaxedIdle);
}

void ATutorialTeamMemberActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bDead)
	{
		UpdateCommandMovement(DeltaSeconds);
		UpdateMovementAnimation();
	}

	if (!NameplateWidget)
	{
		return;
	}

	if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0))
	{
		const FRotator LookAt = (CameraManager->GetCameraLocation() - NameplateWidget->GetComponentLocation()).Rotation();
		NameplateWidget->SetWorldRotation(FRotator(0.0f, LookAt.Yaw, 0.0f));
	}
}

void ATutorialTeamMemberActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bSnapToGroundOnConstruction)
	{
		SnapToGround();
	}
}

void ATutorialTeamMemberActor::InitializeTeamMember(ETeamMemberRole InRole)
{
	TeamRole = InRole;
	RefreshNameplate();
}

void ATutorialTeamMemberActor::SetSelected(bool bInSelected)
{
	if (bDead)
	{
		return;
	}

	const bool bSelectionChanged = bSelected != bInSelected;
	bSelected = bInSelected;
	RefreshNameplate();

	if (bSelectionChanged)
	{
		PlayTeamAnimation(bSelected ? ETeamMemberAnimState::AlertIdle : ETeamMemberAnimState::RelaxedIdle);
	}
}

float ATutorialTeamMemberActor::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (!bCanReceivePlayerWeaponDamage || bDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, FMath::Max(1.0f, MaxHealth));
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitReactionTimerHandle);
	}

	if (CurrentHealth <= KINDA_SMALL_NUMBER)
	{
		bDead = true;
		StopCommandMovement();
		bHasMoveTarget = false;
		PendingBreachDoor = nullptr;

		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->StopMovementImmediately();
			Movement->DisableMovement();
		}

		PlayTeamAnimation(ETeamMemberAnimState::Death);
		return DamageAmount;
	}

	PlayTeamAnimation(ETeamMemberAnimState::HitReact);
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			HitReactionTimerHandle,
			this,
			&ATutorialTeamMemberActor::ResumeAfterHitReaction,
			0.45f,
			false);
	}

	return AppliedDamage > 0.0f ? AppliedDamage : DamageAmount;
}

void ATutorialTeamMemberActor::PlayTeamAnimation(ETeamMemberAnimState NewState)
{
	UAnimSequence* Anim = ResolveAnimation(NewState);
	if (!SoldierMesh || !Anim)
	{
		return;
	}

	if (CurrentAnimState == NewState && CurrentAnimation == Anim)
	{
		return;
	}

	CurrentAnimState = NewState;
	CurrentAnimation = Anim;

	const bool bLoop =
		NewState == ETeamMemberAnimState::RelaxedIdle
		|| NewState == ETeamMemberAnimState::AlertIdle
		|| NewState == ETeamMemberAnimState::CrouchAlert
		|| NewState == ETeamMemberAnimState::Walk
		|| NewState == ETeamMemberAnimState::Run
		|| NewState == ETeamMemberAnimState::CrouchWalk
		|| NewState == ETeamMemberAnimState::Fire;

	SoldierMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	SoldierMesh->SetAnimation(Anim);
	SoldierMesh->Play(bLoop);

	if (NewState == ETeamMemberAnimState::Fire)
	{
		FireWeaponForward();
	}
}

void ATutorialTeamMemberActor::MoveToCommandLocation(const FVector& WorldLocation)
{
	if (bDead)
	{
		return;
	}

	StartMoveToLocation(WorldLocation, true);
}

void ATutorialTeamMemberActor::StopCommandMovement()
{
	PendingBreachDoor = nullptr;
	bHasMoveTarget = false;
	LowSpeedMoveSeconds = 0.0f;

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
	}

	if (!bDead)
	{
		PlayTeamAnimation(bSelected ? ETeamMemberAnimState::AlertIdle : ETeamMemberAnimState::RelaxedIdle);
	}
}

void ATutorialTeamMemberActor::StartMoveToLocation(const FVector& WorldLocation, bool bClearPendingDoor)
{
	if (bDead)
	{
		return;
	}

	const bool bAlreadyMovingToCommand = bHasMoveTarget;
	FVector TargetLocation = WorldLocation;
	if (bClearPendingDoor)
	{
		PendingBreachDoor = nullptr;
	}

	if (UWorld* World = GetWorld())
	{
		if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
		{
			FNavLocation ProjectedLocation;
			const FVector Extent(NavProjectionExtent, NavProjectionExtent, NavProjectionExtent);
			if (NavSystem->ProjectPointToNavigation(WorldLocation, ProjectedLocation, Extent))
			{
				TargetLocation = ProjectedLocation.Location;
			}
		}
	}

	MoveTargetLocation = TargetLocation;
	bHasMoveTarget = true;
	LowSpeedMoveSeconds = 0.0f;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = MovementSpeed;
		Movement->MaxStepHeight = StepUpHeight;
		if (!bAlreadyMovingToCommand)
		{
			Movement->StopMovementImmediately();
		}
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToLocation(TargetLocation, AcceptanceRadius, true, true, true, true, nullptr, true);
	}

	PlayTeamAnimation(ETeamMemberAnimState::Walk);
}

void ATutorialTeamMemberActor::BreachDoor(ATutorialDoorActor* Door)
{
	if (!Door || bDead)
	{
		return;
	}

	const float DistanceSquared = FVector::DistSquared2D(GetActorLocation(), Door->GetActorLocation());
	if (DistanceSquared > FMath::Square(DoorBreachDistance))
	{
		PendingBreachDoor = Door;
		StartMoveToLocation(Door->GetActorLocation(), false);
		return;
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	bHasMoveTarget = false;
	PendingBreachDoor = nullptr;
	SetActorRotation((Door->GetActorLocation() - GetActorLocation()).Rotation());
	PlayTeamAnimation(ETeamMemberAnimState::BreachKick);
	Door->BreachFrom(GetActorLocation());
}

bool ATutorialTeamMemberActor::FireWeaponForward()
{
	FTransform MuzzleTransform;
	if (!GetWeaponMuzzleTransform(MuzzleTransform))
	{
		return false;
	}

	const FTransform CorrectedMuzzleTransform = GetCorrectedWeaponMuzzleTransform(MuzzleTransform);
	FVector ShotDirection = CorrectedMuzzleTransform.GetUnitAxis(EAxis::X).GetSafeNormal();
	if (ShotDirection.IsNearlyZero())
	{
		ShotDirection = -MuzzleTransform.GetUnitAxis(EAxis::X).GetSafeNormal();
	}

	return FireWeaponInDirection(ShotDirection);
}

bool ATutorialTeamMemberActor::FireWeaponAtLocation(const FVector& TargetLocation)
{
	FTransform MuzzleTransform;
	if (!GetWeaponMuzzleTransform(MuzzleTransform))
	{
		return false;
	}

	const FTransform CorrectedMuzzleTransform = GetCorrectedWeaponMuzzleTransform(MuzzleTransform);
	FVector ShotDirection = (TargetLocation - CorrectedMuzzleTransform.GetLocation()).GetSafeNormal();
	if (ShotDirection.IsNearlyZero())
	{
		ShotDirection = CorrectedMuzzleTransform.GetUnitAxis(EAxis::X).GetSafeNormal();
	}

	return FireWeaponInDirection(ShotDirection);
}

void ATutorialTeamMemberActor::SnapToGround()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, GroundTraceUp);
	const FVector End = GetActorLocation() - FVector(0.0f, 0.0f, GroundTraceDown);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ActionSquadSnapToGround), false, this);
	FHitResult Hit;
	if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
	{
		const float CapsuleHalfHeight = GetCapsuleComponent() ? GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : 0.0f;
		SetActorLocation(Hit.ImpactPoint + FVector(0.0f, 0.0f, CapsuleHalfHeight));
	}
}

void ATutorialTeamMemberActor::LoadDefaultAssets()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SoldierMeshAsset(
		TEXT("/Game/Animation/BodyMesh/Swat.Swat"));
	if (SoldierMeshAsset.Succeeded())
	{
		SoldierMesh->SetSkeletalMesh(SoldierMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> RelaxedIdleAsset(
		TEXT("/Game/Animation/Animation/Rifle_Idle.Rifle_Idle"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> AlertIdleAsset(
		TEXT("/Game/Animation/Animation/Idle_Aiming.Idle_Aiming"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> CrouchAlertAsset(
		TEXT("/Game/Animation/Animation/Idle_Crouching_Aiming.Idle_Crouching_Aiming"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> WalkAsset(
		TEXT("/Game/Animation/Animation/Walk_With_Rifle.Walk_With_Rifle"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> RunAsset(
		TEXT("/Game/Animation/Animation/Rifle_Run.Rifle_Run"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> CrouchWalkAsset(
		TEXT("/Game/Animation/Animation/Crouched_Walking.Crouched_Walking"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> FireAsset(
		TEXT("/Game/Animation/Animation/Firing_Rifle.Firing_Rifle"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> ReloadAsset(
		TEXT("/Game/Animation/Animation/Reloading.Reloading"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> HitReactAsset(
		TEXT("/Game/Animation/Animation/Hit_Reaction.Hit_Reaction"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> DeathAsset(
		TEXT("/Game/Animation/Animation/Death.Death"));
	static ConstructorHelpers::FObjectFinder<UAnimSequence> BreachKickAsset(
		TEXT("/Game/Animation/Animation/Rifle_Turn_And_Kick.Rifle_Turn_And_Kick"));

	AnimationSet.RelaxedIdle = RelaxedIdleAsset.Object;
	AnimationSet.AlertIdle = AlertIdleAsset.Object;
	AnimationSet.CrouchAlert = CrouchAlertAsset.Object;
	AnimationSet.Walk = WalkAsset.Object;
	AnimationSet.Run = RunAsset.Object;
	AnimationSet.CrouchWalk = CrouchWalkAsset.Object;
	AnimationSet.Fire = FireAsset.Object;
	AnimationSet.Reload = ReloadAsset.Object;
	AnimationSet.HitReact = HitReactAsset.Object;
	AnimationSet.Death = DeathAsset.Object;
	AnimationSet.BreachKick = BreachKickAsset.Object;
}

void ATutorialTeamMemberActor::RefreshNameplate()
{
	if (!NameplateWidget)
	{
		return;
	}

	if (!NameplateWidget->GetWidget())
	{
		NameplateWidget->InitWidget();
	}

	UTeamNameplateWidget* Widget = Cast<UTeamNameplateWidget>(NameplateWidget->GetWidget());
	if (!Widget)
	{
		return;
	}

	FText Label = FText::FromString(TEXT("A"));
	if (TeamRole == ETeamMemberRole::TeamB)
	{
		Label = FText::FromString(TEXT("B"));
	}
	else if (TeamRole == ETeamMemberRole::Enemy)
	{
		Label = FText::FromString(TEXT("EN"));
	}

	Widget->SetTeamLabel(Label);
	Widget->SetSelected(bSelected);
}

void ATutorialTeamMemberActor::SpawnAndAttachWeapon()
{
	if (!bSpawnWeapon || EquippedWeapon || !SoldierMesh || !GetWorld())
	{
		return;
	}

	UClass* ClassToSpawn = WeaponActorClass ? WeaponActorClass.Get() : ATutorialWeaponActor::StaticClass();
	if (!ClassToSpawn)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	EquippedWeapon = GetWorld()->SpawnActor<ATutorialWeaponActor>(
		ClassToSpawn,
		GetActorLocation(),
		GetActorRotation(),
		SpawnParams);

	if (!EquippedWeapon)
	{
		return;
	}

	const FName AttachSocket = ResolveWeaponAttachSocket();
	EquippedWeapon->AttachToComponent(
		SoldierMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		AttachSocket);
	EquippedWeapon->SetActorRelativeTransform(FTransform::Identity);
}

bool ATutorialTeamMemberActor::FireWeaponInDirection(const FVector& InShotDirection)
{
	UWorld* World = GetWorld();
	if (!World || !bEnableWeaponFire || bDead)
	{
		return false;
	}

	const float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime - LastWeaponFireTime < WeaponFireInterval)
	{
		return false;
	}

	FTransform MuzzleTransform;
	if (!GetWeaponMuzzleTransform(MuzzleTransform))
	{
		return false;
	}

	const FTransform CorrectedMuzzleTransform = GetCorrectedWeaponMuzzleTransform(MuzzleTransform);
	FVector ShotDirection = InShotDirection.GetSafeNormal();
	if (ShotDirection.IsNearlyZero())
	{
		ShotDirection = CorrectedMuzzleTransform.GetUnitAxis(EAxis::X).GetSafeNormal();
	}
	if (ShotDirection.IsNearlyZero())
	{
		ShotDirection = GetActorForwardVector();
	}

	const FVector TraceStart = CorrectedMuzzleTransform.GetLocation();
	const FVector TraceEnd = TraceStart + ShotDirection * WeaponRange;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ActionSquadTeamWeaponTrace), true, this);
	if (EquippedWeapon)
	{
		QueryParams.AddIgnoredActor(EquippedWeapon);
	}

	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	const FVector ShotEnd = bHit ? Hit.ImpactPoint : TraceEnd;

	SpawnWeaponMuzzleFlash(CorrectedMuzzleTransform);
	SpawnWeaponBulletTracer(TraceStart, ShotEnd);

	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		const bool bCharacterImpact = Cast<APawn>(HitActor) != nullptr;
		SpawnWeaponImpactEffect(Hit, bCharacterImpact);
		SpawnWeaponBulletMark(Hit, bCharacterImpact);

		if (HitActor && WeaponDamage > 0.0f)
		{
			UGameplayStatics::ApplyPointDamage(
				HitActor,
				WeaponDamage,
				ShotDirection,
				Hit,
				GetController(),
				this,
				nullptr);
		}
	}

	LastWeaponFireTime = CurrentTime;
	return true;
}

bool ATutorialTeamMemberActor::GetWeaponMuzzleTransform(FTransform& OutMuzzleTransform) const
{
	if (!EquippedWeapon)
	{
		return false;
	}

	OutMuzzleTransform = EquippedWeapon->GetMuzzleTransform();
	return true;
}

FTransform ATutorialTeamMemberActor::GetCorrectedWeaponMuzzleTransform(const FTransform& MuzzleTransform) const
{
	const FVector ReversedForward = -MuzzleTransform.GetUnitAxis(EAxis::X).GetSafeNormal();
	if (ReversedForward.IsNearlyZero())
	{
		return MuzzleTransform;
	}

	const FVector OriginalUp = MuzzleTransform.GetUnitAxis(EAxis::Z).GetSafeNormal();
	const FVector CorrectedUp = OriginalUp.IsNearlyZero() || FMath::Abs(FVector::DotProduct(OriginalUp, ReversedForward)) > 0.98f
		? FVector::UpVector
		: OriginalUp;

	FTransform CorrectedTransform = MuzzleTransform;
	CorrectedTransform.SetRotation(FRotationMatrix::MakeFromXZ(ReversedForward, CorrectedUp).ToQuat());
	CorrectedTransform.NormalizeRotation();
	return CorrectedTransform;
}

void ATutorialTeamMemberActor::SpawnWeaponMuzzleFlash(const FTransform& MuzzleTransform)
{
	if (!bSpawnWeaponMuzzleFlash)
	{
		return;
	}

	UWorld* World = GetWorld();
	UClass* EffectClass = MuzzleFlashEffectClass ? MuzzleFlashEffectClass.Get() : ATutorialBallisticEffectActor::StaticClass();
	if (!World || !EffectClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATutorialBallisticEffectActor* Effect = World->SpawnActor<ATutorialBallisticEffectActor>(EffectClass, MuzzleTransform, SpawnParams);
	if (Effect)
	{
		Effect->ConfigureMuzzleFlash(MuzzleTransform);
	}
}

void ATutorialTeamMemberActor::SpawnWeaponBulletTracer(const FVector& StartLocation, const FVector& EndLocation)
{
	if (!bSpawnWeaponBulletTracer)
	{
		return;
	}

	UWorld* World = GetWorld();
	UClass* EffectClass = BulletTracerEffectClass ? BulletTracerEffectClass.Get() : ATutorialBallisticEffectActor::StaticClass();
	if (!World || !EffectClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATutorialBallisticEffectActor* Effect = World->SpawnActor<ATutorialBallisticEffectActor>(
		EffectClass,
		StartLocation,
		FRotator::ZeroRotator,
		SpawnParams);
	if (Effect)
	{
		Effect->ConfigureBulletTracer(StartLocation, EndLocation);
	}
}

void ATutorialTeamMemberActor::SpawnWeaponImpactEffect(const FHitResult& Hit, bool bCharacterImpact)
{
	if (!bSpawnWeaponImpactEffect)
	{
		return;
	}

	UWorld* World = GetWorld();
	UClass* EffectClass = ImpactEffectClass ? ImpactEffectClass.Get() : ATutorialBallisticEffectActor::StaticClass();
	if (!World || !EffectClass)
	{
		return;
	}

	const FVector ImpactNormal = Hit.ImpactNormal.IsNearlyZero() ? FVector::UpVector : Hit.ImpactNormal.GetSafeNormal();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATutorialBallisticEffectActor* Effect = World->SpawnActor<ATutorialBallisticEffectActor>(
		EffectClass,
		Hit.ImpactPoint + ImpactNormal,
		ImpactNormal.Rotation(),
		SpawnParams);
	if (Effect)
	{
		Effect->ConfigureImpact(Hit.ImpactPoint, ImpactNormal, bCharacterImpact);
	}
}

void ATutorialTeamMemberActor::SpawnWeaponBulletMark(const FHitResult& Hit, bool bCharacterMark)
{
	if (!bSpawnWeaponBulletMarks)
	{
		return;
	}

	UWorld* World = GetWorld();
	UClass* MarkClass = BulletMarkClass ? BulletMarkClass.Get() : ATutorialBulletMarkActor::StaticClass();
	if (!World || !MarkClass)
	{
		return;
	}

	FVector ImpactNormal = Hit.ImpactNormal.GetSafeNormal();
	if (ImpactNormal.IsNearlyZero())
	{
		ImpactNormal = FVector::UpVector;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATutorialBulletMarkActor* Mark = World->SpawnActor<ATutorialBulletMarkActor>(
		MarkClass,
		Hit.ImpactPoint + ImpactNormal * 0.5f,
		ImpactNormal.Rotation(),
		SpawnParams);
	if (!Mark)
	{
		return;
	}

	Mark->ConfigureBulletMark(Hit.ImpactPoint, ImpactNormal, Hit.GetComponent(), bCharacterMark);
	WeaponBulletMarks.Add(Mark);
	PruneWeaponBulletMarks();
}

void ATutorialTeamMemberActor::PruneWeaponBulletMarks()
{
	WeaponBulletMarks.RemoveAll([](const TWeakObjectPtr<ATutorialBulletMarkActor>& Mark)
	{
		return !Mark.IsValid();
	});

	if (MaxWeaponBulletMarks <= 0)
	{
		for (const TWeakObjectPtr<ATutorialBulletMarkActor>& Mark : WeaponBulletMarks)
		{
			if (Mark.IsValid())
			{
				Mark->Destroy();
			}
		}
		WeaponBulletMarks.Reset();
		return;
	}

	while (WeaponBulletMarks.Num() > MaxWeaponBulletMarks)
	{
		if (ATutorialBulletMarkActor* OldestMark = WeaponBulletMarks[0].Get())
		{
			OldestMark->Destroy();
		}
		WeaponBulletMarks.RemoveAt(0);
	}
}

FName ATutorialTeamMemberActor::ResolveWeaponAttachSocket() const
{
	if (SoldierMesh && WeaponSocketName != NAME_None && SoldierMesh->DoesSocketExist(WeaponSocketName))
	{
		return WeaponSocketName;
	}

	if (SoldierMesh && FallbackWeaponBoneName != NAME_None && SoldierMesh->GetBoneIndex(FallbackWeaponBoneName) != INDEX_NONE)
	{
		return FallbackWeaponBoneName;
	}

	return NAME_None;
}

UAnimSequence* ATutorialTeamMemberActor::ResolveAnimation(ETeamMemberAnimState State) const
{
	switch (State)
	{
	case ETeamMemberAnimState::RelaxedIdle:
		return AnimationSet.RelaxedIdle;
	case ETeamMemberAnimState::AlertIdle:
		return AnimationSet.AlertIdle ? AnimationSet.AlertIdle : AnimationSet.RelaxedIdle;
	case ETeamMemberAnimState::CrouchAlert:
		return AnimationSet.CrouchAlert ? AnimationSet.CrouchAlert : AnimationSet.AlertIdle;
	case ETeamMemberAnimState::Walk:
		return AnimationSet.Walk;
	case ETeamMemberAnimState::Run:
		return AnimationSet.Run ? AnimationSet.Run : AnimationSet.Walk;
	case ETeamMemberAnimState::CrouchWalk:
		return AnimationSet.CrouchWalk ? AnimationSet.CrouchWalk : AnimationSet.Walk;
	case ETeamMemberAnimState::Fire:
		return AnimationSet.Fire ? AnimationSet.Fire : AnimationSet.AlertIdle;
	case ETeamMemberAnimState::Reload:
		return AnimationSet.Reload;
	case ETeamMemberAnimState::HitReact:
		return AnimationSet.HitReact;
	case ETeamMemberAnimState::Death:
		return AnimationSet.Death;
	case ETeamMemberAnimState::BreachKick:
		return AnimationSet.BreachKick;
	default:
		return AnimationSet.RelaxedIdle;
	}
}

void ATutorialTeamMemberActor::UpdateCommandMovement(float DeltaSeconds)
{
	if (!bHasMoveTarget)
	{
		return;
	}

	const float Distance = FVector::Dist2D(GetActorLocation(), MoveTargetLocation);
	const float StopRadius = FMath::Max(AcceptanceRadius, GetCapsuleComponent() ? GetCapsuleComponent()->GetScaledCapsuleRadius() * 1.5f : AcceptanceRadius);
	if (Distance <= StopRadius)
	{
		FinishMoveCommand();
		return;
	}

	const AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetMoveStatus() != EPathFollowingStatus::Moving && Distance <= StopRadius * 2.5f)
	{
		FinishMoveCommand();
		return;
	}

	const float Speed2D = GetVelocity().Size2D();
	if (Speed2D <= MovementSpeed * 0.08f)
	{
		LowSpeedMoveSeconds += FMath::Max(0.0f, DeltaSeconds);
	}
	else
	{
		LowSpeedMoveSeconds = 0.0f;
	}

	if (LowSpeedMoveSeconds >= LowSpeedStopSeconds && Distance <= StopRadius * 2.5f)
	{
		FinishMoveCommand();
	}
}

void ATutorialTeamMemberActor::FinishMoveCommand()
{
	bHasMoveTarget = false;
	LowSpeedMoveSeconds = 0.0f;
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
	}
	SnapToGround();

	if (PendingBreachDoor)
	{
		ATutorialDoorActor* DoorToBreach = PendingBreachDoor;
		PendingBreachDoor = nullptr;
		if (DoorToBreach && FVector::DistSquared2D(GetActorLocation(), DoorToBreach->GetActorLocation()) <= FMath::Square(DoorBreachDistance * 1.5f))
		{
			BreachDoor(DoorToBreach);
			return;
		}
	}

	PlayTeamAnimation(bSelected ? ETeamMemberAnimState::AlertIdle : ETeamMemberAnimState::RelaxedIdle);
}

void ATutorialTeamMemberActor::UpdateMovementAnimation()
{
	if (!bHasMoveTarget || CurrentAnimState == ETeamMemberAnimState::HitReact)
	{
		return;
	}

	const float Speed2D = GetVelocity().Size2D();
	if (Speed2D > MovementSpeed * 0.2f)
	{
		PlayTeamAnimation(ETeamMemberAnimState::Walk);
	}
	else if (CurrentAnimState == ETeamMemberAnimState::Walk && LowSpeedMoveSeconds >= LowSpeedStopSeconds)
	{
		FinishMoveCommand();
	}
}

void ATutorialTeamMemberActor::ResumeAfterHitReaction()
{
	if (bDead)
	{
		return;
	}

	if (bHasMoveTarget)
	{
		PlayTeamAnimation(ETeamMemberAnimState::Walk);
	}
	else
	{
		PlayTeamAnimation(bSelected ? ETeamMemberAnimState::AlertIdle : ETeamMemberAnimState::RelaxedIdle);
	}
}

ATutorialTeamAActor::ATutorialTeamAActor()
{
	TeamRole = ETeamMemberRole::TeamA;
}

ATutorialTeamBActor::ATutorialTeamBActor()
{
	TeamRole = ETeamMemberRole::TeamB;
}
