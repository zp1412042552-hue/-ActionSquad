#include "TutorialBulletMarkActor.h"

#include "Components/PrimitiveComponent.h"
#include "ProceduralMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATutorialBulletMarkActor::ATutorialBulletMarkActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	MarkMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MarkMesh"));
	MarkMesh->SetupAttachment(SceneRoot);
	MarkMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarkMesh->SetGenerateOverlapEvents(false);
	MarkMesh->SetCanEverAffectNavigation(false);
	MarkMesh->bUseAsyncCooking = true;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> VertexColorMaterial(
		TEXT("/Engine/EngineDebugMaterials/VertexColorMaterial.VertexColorMaterial"));
	if (VertexColorMaterial.Succeeded())
	{
		MarkMaterial = VertexColorMaterial.Object;
	}
}

void ATutorialBulletMarkActor::ConfigureBulletMark(
	const FVector& ImpactLocation,
	const FVector& ImpactNormal,
	UPrimitiveComponent* HitComponent,
	bool bCharacterMark)
{
	FVector Normal = ImpactNormal.GetSafeNormal();
	if (Normal.IsNearlyZero())
	{
		Normal = FVector::UpVector;
	}

	const float Offset = bCharacterMark ? CharacterOffset : SurfaceOffset;
	const float Radius = bCharacterMark ? CharacterRadius : SurfaceRadius;
	const FLinearColor CoreColor = bCharacterMark ? CharacterCoreColor : SurfaceCoreColor;
	const FLinearColor EdgeColor = bCharacterMark ? CharacterEdgeColor : SurfaceEdgeColor;
	const float LifeSeconds = bCharacterMark ? CharacterLifeSeconds : SurfaceLifeSeconds;

	const int32 Seed =
		FMath::RoundToInt(ImpactLocation.X * 13.0f) ^
		FMath::RoundToInt(ImpactLocation.Y * 17.0f) ^
		FMath::RoundToInt(ImpactLocation.Z * 19.0f);
	FRotator MarkRotation = Normal.Rotation();
	MarkRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

	SetActorLocation(ImpactLocation + Normal * Offset);
	SetActorRotation(MarkRotation);
	BuildMarkMesh(Radius, CoreColor, EdgeColor, Seed);
	SetLifeSpan(LifeSeconds);

	if (HitComponent)
	{
		AttachToComponent(HitComponent, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void ATutorialBulletMarkActor::BuildMarkMesh(float Radius, const FLinearColor& CoreColor, const FLinearColor& EdgeColor, int32 Seed)
{
	if (!MarkMesh)
	{
		return;
	}

	const int32 Segments = 18;
	const float InnerRadius = Radius * 0.42f;
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FLinearColor> Colors;

	Vertices.Add(FVector::ZeroVector);
	Colors.Add(CoreColor);

	for (int32 Index = 0; Index < Segments; ++Index)
	{
		const float Angle = 2.0f * PI * static_cast<float>(Index) / static_cast<float>(Segments);
		Vertices.Add(FVector(0.0f, FMath::Cos(Angle) * InnerRadius, FMath::Sin(Angle) * InnerRadius));
		Colors.Add(CoreColor);
	}

	for (int32 Index = 0; Index < Segments; ++Index)
	{
		const float Angle = 2.0f * PI * static_cast<float>(Index) / static_cast<float>(Segments);
		const float Variation = 0.76f + 0.24f * FMath::Abs(FMath::Sin(static_cast<float>(Seed + Index * 31) * 0.37f));
		Vertices.Add(FVector(0.0f, FMath::Cos(Angle) * Radius * Variation, FMath::Sin(Angle) * Radius * Variation));
		Colors.Add(EdgeColor);
	}

	for (int32 Index = 0; Index < Segments; ++Index)
	{
		const int32 InnerCurrent = Index + 1;
		const int32 InnerNext = ((Index + 1) % Segments) + 1;
		const int32 OuterCurrent = Segments + Index + 1;
		const int32 OuterNext = Segments + ((Index + 1) % Segments) + 1;

		Triangles.Add(0);
		Triangles.Add(InnerCurrent);
		Triangles.Add(InnerNext);

		Triangles.Add(InnerCurrent);
		Triangles.Add(OuterCurrent);
		Triangles.Add(OuterNext);

		Triangles.Add(InnerCurrent);
		Triangles.Add(OuterNext);
		Triangles.Add(InnerNext);
	}

	MarkMesh->ClearAllMeshSections();
	MarkMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, {}, {}, Colors, {}, false);
	ApplyMarkMaterial();
}

void ATutorialBulletMarkActor::ApplyMarkMaterial()
{
	if (MarkMesh && MarkMaterial)
	{
		MarkMesh->SetMaterial(0, MarkMaterial);
	}
}
