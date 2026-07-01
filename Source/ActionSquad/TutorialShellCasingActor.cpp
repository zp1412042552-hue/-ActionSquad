#include "TutorialShellCasingActor.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ATutorialShellCasingActor::ATutorialShellCasingActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShellMesh"));
	SetRootComponent(ShellMesh);
	ShellMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	ShellMesh->SetGenerateOverlapEvents(false);
	ShellMesh->SetCanEverAffectNavigation(false);
	ShellMesh->SetSimulatePhysics(false);
	ShellMesh->SetRelativeScale3D(ShellMeshScale);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		ShellMesh->SetStaticMesh(CylinderMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BasicShapeMaterial(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BasicShapeMaterial.Succeeded())
	{
		ShellMaterial = BasicShapeMaterial.Object;
		ShellMesh->SetMaterial(0, ShellMaterial);
	}
}

void ATutorialShellCasingActor::LaunchShell(
	const FTransform& EjectionTransform,
	const FVector& LinearVelocity,
	const FVector& AngularVelocityDegrees)
{
	SetActorTransform(EjectionTransform);
	SetLifeSpan(ShellLifeSeconds);

	if (!ShellMesh)
	{
		return;
	}

	ShellMesh->SetRelativeScale3D(ShellMeshScale);

	if (ShellMaterial)
	{
		UMaterialInstanceDynamic* DynamicMaterial = ShellMesh->CreateAndSetMaterialInstanceDynamicFromMaterial(0, ShellMaterial);
		if (DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), ShellTint);
			DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), ShellTint);
		}
	}

	ShellMesh->SetSimulatePhysics(true);
	ShellMesh->SetPhysicsLinearVelocity(LinearVelocity);
	ShellMesh->SetPhysicsAngularVelocityInDegrees(AngularVelocityDegrees);
}
