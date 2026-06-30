#include "TutorialCommandAimVisualActor.h"

#include "Components/SceneComponent.h"
#include "Materials/MaterialInterface.h"
#include "ProceduralMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATutorialCommandAimVisualActor::ATutorialCommandAimVisualActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BeamMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("BeamMesh"));
	BeamMesh->SetupAttachment(SceneRoot);
	BeamMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BeamMesh->SetGenerateOverlapEvents(false);

	ImpactMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ImpactMesh"));
	ImpactMesh->SetupAttachment(SceneRoot);
	ImpactMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ImpactMesh->SetGenerateOverlapEvents(false);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> VertexColorMaterialAsset(
		TEXT("/Engine/EngineDebugMaterials/VertexColorMaterial.VertexColorMaterial"));
	if (VertexColorMaterialAsset.Succeeded())
	{
		VertexColorMaterial = VertexColorMaterialAsset.Object;
		BeamMesh->SetMaterial(0, VertexColorMaterial);
		ImpactMesh->SetMaterial(0, VertexColorMaterial);
	}

	SetActorHiddenInGame(true);
}

void ATutorialCommandAimVisualActor::ShowAim(ESelectedTeamTarget Target, const FVector& Start, const FVector& End, bool bValidTarget)
{
	const FVector Ray = End - Start;
	const float Length = Ray.Size();
	if (Length <= KINDA_SMALL_NUMBER)
	{
		HideAim();
		return;
	}

	const FLinearColor Color = ResolveColor(Target, bValidTarget);
	SetActorLocation(Start);
	SetActorRotation(FRotationMatrix::MakeFromX(Ray / Length).Rotator());
	BuildBeamMesh(Length, Color);

	ImpactMesh->SetRelativeLocation(FVector(Length, 0.0f, 0.0f));
	BuildImpactMesh(Color);

	SetActorHiddenInGame(false);
}

void ATutorialCommandAimVisualActor::HideAim()
{
	SetActorHiddenInGame(true);
	if (BeamMesh)
	{
		BeamMesh->ClearAllMeshSections();
	}
	if (ImpactMesh)
	{
		ImpactMesh->ClearAllMeshSections();
	}
}

void ATutorialCommandAimVisualActor::BuildBeamMesh(float Length, const FLinearColor& Color)
{
	if (!BeamMesh)
	{
		return;
	}

	const int32 NumSegments = FMath::Clamp(SegmentCount, 6, 32);
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> Colors;
	TArray<FProcMeshTangent> Tangents;

	Vertices.Reserve(NumSegments * 2);
	Normals.Reserve(NumSegments * 2);
	UVs.Reserve(NumSegments * 2);
	Colors.Reserve(NumSegments * 2);
	Tangents.Reserve(NumSegments * 2);
	Triangles.Reserve(NumSegments * 6);

	for (int32 SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
	{
		const float Angle = (2.0f * PI * static_cast<float>(SegmentIndex)) / static_cast<float>(NumSegments);
		const float Y = FMath::Cos(Angle) * BeamRadius;
		const float Z = FMath::Sin(Angle) * BeamRadius;
		const FVector Normal(0.0f, FMath::Cos(Angle), FMath::Sin(Angle));

		Vertices.Add(FVector(0.0f, Y, Z));
		Vertices.Add(FVector(Length, Y, Z));
		Normals.Add(Normal);
		Normals.Add(Normal);
		UVs.Add(FVector2D(0.0f, SegmentIndex / static_cast<float>(NumSegments)));
		UVs.Add(FVector2D(1.0f, SegmentIndex / static_cast<float>(NumSegments)));
		Colors.Add(Color);
		Colors.Add(Color);
		Tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
		Tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
	}

	for (int32 SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
	{
		const int32 NextSegmentIndex = (SegmentIndex + 1) % NumSegments;
		const int32 A0 = SegmentIndex * 2;
		const int32 A1 = A0 + 1;
		const int32 B0 = NextSegmentIndex * 2;
		const int32 B1 = B0 + 1;

		Triangles.Add(A0);
		Triangles.Add(B0);
		Triangles.Add(A1);
		Triangles.Add(A1);
		Triangles.Add(B0);
		Triangles.Add(B1);
	}

	BeamMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, Colors, Tangents, false);
	if (VertexColorMaterial)
	{
		BeamMesh->SetMaterial(0, VertexColorMaterial);
	}
}

void ATutorialCommandAimVisualActor::BuildImpactMesh(const FLinearColor& Color)
{
	if (!ImpactMesh)
	{
		return;
	}

	const int32 NumSegments = FMath::Clamp(SegmentCount, 6, 32);
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> Colors;
	TArray<FProcMeshTangent> Tangents;

	Vertices.Reserve(NumSegments + 1);
	Normals.Reserve(NumSegments + 1);
	UVs.Reserve(NumSegments + 1);
	Colors.Reserve(NumSegments + 1);
	Tangents.Reserve(NumSegments + 1);
	Triangles.Reserve(NumSegments * 3);

	Vertices.Add(FVector::ZeroVector);
	Normals.Add(FVector::XAxisVector);
	UVs.Add(FVector2D(0.5f, 0.5f));
	Colors.Add(Color);
	Tangents.Add(FProcMeshTangent(0.0f, 1.0f, 0.0f));

	for (int32 SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
	{
		const float Angle = (2.0f * PI * static_cast<float>(SegmentIndex)) / static_cast<float>(NumSegments);
		const float Y = FMath::Cos(Angle) * ImpactRadius;
		const float Z = FMath::Sin(Angle) * ImpactRadius;
		Vertices.Add(FVector(0.0f, Y, Z));
		Normals.Add(FVector::XAxisVector);
		UVs.Add(FVector2D(0.5f + FMath::Cos(Angle) * 0.5f, 0.5f + FMath::Sin(Angle) * 0.5f));
		Colors.Add(Color);
		Tangents.Add(FProcMeshTangent(0.0f, 1.0f, 0.0f));
	}

	for (int32 SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
	{
		const int32 NextSegmentIndex = (SegmentIndex + 1) % NumSegments;
		Triangles.Add(0);
		Triangles.Add(1 + SegmentIndex);
		Triangles.Add(1 + NextSegmentIndex);
	}

	ImpactMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, Colors, Tangents, false);
	if (VertexColorMaterial)
	{
		ImpactMesh->SetMaterial(0, VertexColorMaterial);
	}
}

FLinearColor ATutorialCommandAimVisualActor::ResolveColor(ESelectedTeamTarget Target, bool bValidTarget) const
{
	if (!bValidTarget)
	{
		return InvalidColor;
	}

	return Target == ESelectedTeamTarget::TeamB ? TeamBColor : TeamAColor;
}
