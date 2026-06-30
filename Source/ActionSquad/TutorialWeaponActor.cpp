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
}

FTransform ATutorialWeaponActor::GetMuzzleTransform() const
{
	return MuzzlePoint ? MuzzlePoint->GetComponentTransform() : GetActorTransform();
}
