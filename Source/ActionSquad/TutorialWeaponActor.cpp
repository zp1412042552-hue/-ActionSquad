#include "TutorialWeaponActor.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATutorialWeaponActor::ATutorialWeaponActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(SceneRoot);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetGenerateOverlapEvents(false);
	WeaponMesh->SetCanEverAffectNavigation(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> GunMeshAsset(TEXT("/Game/Animation/GunMesh/Gun.Gun"));
	if (GunMeshAsset.Succeeded())
	{
		WeaponMesh->SetStaticMesh(GunMeshAsset.Object);
	}

	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(WeaponMesh);
	MuzzlePoint->SetRelativeLocation(FVector(45.0f, 0.0f, 2.0f));

	ShellEjectionPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ShellEjectionPoint"));
	ShellEjectionPoint->SetupAttachment(WeaponMesh);
	ShellEjectionPoint->SetRelativeLocation(FVector(22.0f, 7.0f, 3.0f));
	ShellEjectionPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
}

FTransform ATutorialWeaponActor::GetMuzzleTransform() const
{
	return MuzzlePoint ? MuzzlePoint->GetComponentTransform() : GetActorTransform();
}

FTransform ATutorialWeaponActor::GetFiringMuzzleTransform() const
{
	const FTransform MuzzleTransform = GetMuzzleTransform();
	const FVector ReversedForward = -MuzzleTransform.GetUnitAxis(EAxis::X).GetSafeNormal();
	if (ReversedForward.IsNearlyZero())
	{
		return MuzzleTransform;
	}

	const FVector OriginalUp = MuzzleTransform.GetUnitAxis(EAxis::Z).GetSafeNormal();
	const FVector CorrectedUp = OriginalUp.IsNearlyZero() || FMath::Abs(FVector::DotProduct(OriginalUp, ReversedForward)) > 0.98f
		? FVector::UpVector
		: OriginalUp;

	FTransform FiringTransform = MuzzleTransform;
	FiringTransform.SetRotation(FRotationMatrix::MakeFromXZ(ReversedForward, CorrectedUp).ToQuat());
	FiringTransform.NormalizeRotation();
	return FiringTransform;
}

FTransform ATutorialWeaponActor::GetShellEjectionTransform() const
{
	return ShellEjectionPoint ? ShellEjectionPoint->GetComponentTransform() : GetMuzzleTransform();
}
