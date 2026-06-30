#include "TutorialCommandMarkerActor.h"

#include "Materials/MaterialInterface.h"
#include "ProceduralMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"

ATutorialCommandMarkerActor::ATutorialCommandMarkerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	RingMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RingMesh"));
	RingMesh->SetupAttachment(SceneRoot);
	RingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RingMesh->SetGenerateOverlapEvents(false);

	FillMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("FillMesh"));
	FillMesh->SetupAttachment(SceneRoot);
	FillMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.4f));
	FillMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FillMesh->SetGenerateOverlapEvents(false);

	LabelText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LabelText"));
	LabelText->SetupAttachment(SceneRoot);
	LabelText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	LabelText->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	LabelText->SetRelativeLocation(FVector(0.0f, 0.0f, 2.0f));
	LabelText->SetWorldSize(34.0f);
	LabelText->SetTextRenderColor(FColor::Cyan);
	LabelText->SetText(FText::FromString(TEXT("A")));

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> VertexColorMaterialAsset(
		TEXT("/Engine/EngineDebugMaterials/VertexColorMaterial.VertexColorMaterial"));
	if (VertexColorMaterialAsset.Succeeded())
	{
		VertexColorMaterial = VertexColorMaterialAsset.Object;
		RingMesh->SetMaterial(0, VertexColorMaterial);
		FillMesh->SetMaterial(0, VertexColorMaterial);
	}

	SetActorHiddenInGame(true);
}

void ATutorialCommandMarkerActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsHidden())
	{
		return;
	}

}

void ATutorialCommandMarkerActor::ShowMarker(ESelectedTeamTarget Target, const FVector& Location, const FVector& SurfaceNormal, const FVector& AimDirection, float Progress, bool bCanConfirm)
{
	DisplayTarget = Target;
	bMarkerCanConfirm = bCanConfirm;
	HoldProgress = bCanConfirm ? Progress : 0.0f;
	MarkerNormal = SurfaceNormal.GetSafeNormal();
	if (MarkerNormal.IsNearlyZero())
	{
		MarkerNormal = FVector::UpVector;
	}

	MarkerForward = FVector::VectorPlaneProject(AimDirection, MarkerNormal).GetSafeNormal();
	if (MarkerForward.IsNearlyZero())
	{
		MarkerForward = FVector::VectorPlaneProject(FVector::ForwardVector, MarkerNormal).GetSafeNormal();
	}
	if (MarkerForward.IsNearlyZero())
	{
		MarkerForward = FVector::RightVector;
	}

	SetActorLocation(Location + MarkerNormal * 4.0f);
	SetActorRotation(FRotationMatrix::MakeFromXZ(MarkerForward, MarkerNormal).Rotator());
	SetActorHiddenInGame(false);
	RebuildMarkerMeshes();

	if (LabelText)
	{
		const bool bTeamB = Target == ESelectedTeamTarget::TeamB;
		LabelText->SetText(FText::FromString(bTeamB ? TEXT("B") : TEXT("A")));
		LabelText->SetTextRenderColor(bCanConfirm ? (bTeamB ? FColor(255, 180, 40) : FColor::Cyan) : FColor(230, 55, 55));
	}
}

void ATutorialCommandMarkerActor::HideMarker()
{
	SetActorHiddenInGame(true);
	DisplayTarget = ESelectedTeamTarget::None;
	HoldProgress = 0.0f;
	bMarkerCanConfirm = false;
	if (RingMesh)
	{
		RingMesh->ClearAllMeshSections();
	}
	if (FillMesh)
	{
		FillMesh->ClearAllMeshSections();
	}
}

void ATutorialCommandMarkerActor::RebuildMarkerMeshes()
{
	const FLinearColor MarkerColor = ResolveMarkerColor();
	BuildRingMesh(MarkerColor);

	const float FillRadius = FMath::Max(4.0f, RingRadius * 0.18f);
	BuildFillMesh(FillRadius, bMarkerCanConfirm ? FLinearColor::White : MarkerColor);
}

void ATutorialCommandMarkerActor::BuildRingMesh(const FLinearColor& Color)
{
	if (!RingMesh)
	{
		return;
	}

	constexpr int32 NumSegments = 64;
	const float OuterRadius = RingRadius;
	const float InnerRadius = FMath::Max(1.0f, RingRadius - RingThickness);

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> Colors;
	TArray<FProcMeshTangent> Tangents;

	Vertices.Reserve(NumSegments * 2);
	Triangles.Reserve(NumSegments * 6);
	Normals.Reserve(NumSegments * 2);
	UVs.Reserve(NumSegments * 2);
	Colors.Reserve(NumSegments * 2);
	Tangents.Reserve(NumSegments * 2);

	for (int32 SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
	{
		const float Angle = (2.0f * PI * static_cast<float>(SegmentIndex)) / static_cast<float>(NumSegments);
		const float CosAngle = FMath::Cos(Angle);
		const float SinAngle = FMath::Sin(Angle);

		Vertices.Add(FVector(CosAngle * OuterRadius, SinAngle * OuterRadius, 0.0f));
		Vertices.Add(FVector(CosAngle * InnerRadius, SinAngle * InnerRadius, 0.0f));
		Normals.Add(FVector::UpVector);
		Normals.Add(FVector::UpVector);
		UVs.Add(FVector2D(0.5f + CosAngle * 0.5f, 0.5f + SinAngle * 0.5f));
		UVs.Add(FVector2D(0.5f + CosAngle * 0.35f, 0.5f + SinAngle * 0.35f));
		Colors.Add(Color);
		Colors.Add(Color);
		Tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
		Tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
	}

	for (int32 SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
	{
		const int32 NextSegmentIndex = (SegmentIndex + 1) % NumSegments;
		const int32 OuterA = SegmentIndex * 2;
		const int32 InnerA = OuterA + 1;
		const int32 OuterB = NextSegmentIndex * 2;
		const int32 InnerB = OuterB + 1;

		Triangles.Add(OuterA);
		Triangles.Add(OuterB);
		Triangles.Add(InnerA);
		Triangles.Add(InnerA);
		Triangles.Add(OuterB);
		Triangles.Add(InnerB);
	}

	RingMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, Colors, Tangents, false);
	if (VertexColorMaterial)
	{
		RingMesh->SetMaterial(0, VertexColorMaterial);
	}
}

void ATutorialCommandMarkerActor::BuildFillMesh(float Radius, const FLinearColor& Color)
{
	if (!FillMesh)
	{
		return;
	}

	constexpr int32 NumSegments = 48;
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> Colors;
	TArray<FProcMeshTangent> Tangents;

	Vertices.Reserve(NumSegments + 1);
	Triangles.Reserve(NumSegments * 3);
	Normals.Reserve(NumSegments + 1);
	UVs.Reserve(NumSegments + 1);
	Colors.Reserve(NumSegments + 1);
	Tangents.Reserve(NumSegments + 1);

	Vertices.Add(FVector::ZeroVector);
	Normals.Add(FVector::UpVector);
	UVs.Add(FVector2D(0.5f, 0.5f));
	Colors.Add(Color);
	Tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));

	for (int32 SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
	{
		const float Angle = (2.0f * PI * static_cast<float>(SegmentIndex)) / static_cast<float>(NumSegments);
		const float CosAngle = FMath::Cos(Angle);
		const float SinAngle = FMath::Sin(Angle);
		Vertices.Add(FVector(CosAngle * Radius, SinAngle * Radius, 0.0f));
		Normals.Add(FVector::UpVector);
		UVs.Add(FVector2D(0.5f + CosAngle * 0.5f, 0.5f + SinAngle * 0.5f));
		Colors.Add(Color);
		Tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
	}

	for (int32 SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
	{
		const int32 NextSegmentIndex = (SegmentIndex + 1) % NumSegments;
		Triangles.Add(0);
		Triangles.Add(1 + SegmentIndex);
		Triangles.Add(1 + NextSegmentIndex);
	}

	FillMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, Colors, Tangents, false);
	if (VertexColorMaterial)
	{
		FillMesh->SetMaterial(0, VertexColorMaterial);
	}
}

FLinearColor ATutorialCommandMarkerActor::ResolveMarkerColor() const
{
	if (!bMarkerCanConfirm)
	{
		return FLinearColor(1.0f, 0.08f, 0.05f, 1.0f);
	}

	return DisplayTarget == ESelectedTeamTarget::TeamB
		? FLinearColor(1.0f, 0.58f, 0.08f, 1.0f)
		: FLinearColor(0.0f, 0.86f, 1.0f, 1.0f);
}
