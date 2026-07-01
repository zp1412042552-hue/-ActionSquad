#include "TutorialBallisticEffectActor.h"

#include "Components/PointLightComponent.h"
#include "ProceduralMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATutorialBallisticEffectActor::ATutorialBallisticEffectActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	EffectMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("EffectMesh"));
	EffectMesh->SetupAttachment(SceneRoot);
	EffectMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EffectMesh->SetGenerateOverlapEvents(false);
	EffectMesh->SetCanEverAffectNavigation(false);
	EffectMesh->bUseAsyncCooking = true;

	FlashLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FlashLight"));
	FlashLight->SetupAttachment(SceneRoot);
	FlashLight->SetVisibility(false);
	FlashLight->SetIntensity(0.0f);
	FlashLight->SetAttenuationRadius(140.0f);
	FlashLight->SetLightColor(FLinearColor(1.0f, 0.62f, 0.22f));

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> VertexColorMaterial(
		TEXT("/Engine/EngineDebugMaterials/VertexColorMaterial.VertexColorMaterial"));
	if (VertexColorMaterial.Succeeded())
	{
		EffectMaterial = VertexColorMaterial.Object;
	}
}

void ATutorialBallisticEffectActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ElapsedSeconds += FMath::Max(0.0f, DeltaSeconds);
	if (ElapsedSeconds >= LifeSeconds)
	{
		Destroy();
		return;
	}

	const float Alpha = LifeSeconds > KINDA_SMALL_NUMBER
		? FMath::Clamp(1.0f - ElapsedSeconds / LifeSeconds, 0.0f, 1.0f)
		: 0.0f;

	if (EffectMesh)
	{
		if (EffectType == ETutorialBallisticEffectType::BulletTracer)
		{
			EffectMesh->SetRelativeScale3D(FVector(1.0f, Alpha, Alpha));
		}
		else if (EffectType == ETutorialBallisticEffectType::MuzzleFlash)
		{
			EffectMesh->SetRelativeScale3D(FVector(1.0f, Alpha, Alpha) * InitialMeshScale);
		}
		else
		{
			EffectMesh->SetRelativeScale3D(FMath::Lerp(InitialMeshScale * 0.85f, InitialMeshScale * 1.15f, Alpha));
		}
	}

	if (FlashLight)
	{
		FlashLight->SetIntensity(InitialLightIntensity * Alpha);
	}
}

void ATutorialBallisticEffectActor::ConfigureMuzzleFlash(const FTransform& MuzzleTransform)
{
	EffectType = ETutorialBallisticEffectType::MuzzleFlash;
	LifeSeconds = MuzzleFlashLifeSeconds;
	ElapsedSeconds = 0.0f;
	SetActorTransform(MuzzleTransform);
	BuildConeMesh(MuzzleFlashLength, MuzzleFlashRadius, MuzzleFlashColor);
	InitialMeshScale = FVector::OneVector;

	if (FlashLight)
	{
		InitialLightIntensity = 4200.0f;
		FlashLight->SetVisibility(true);
		FlashLight->SetIntensity(InitialLightIntensity);
		FlashLight->SetRelativeLocation(FVector(MuzzleFlashLength * 0.35f, 0.0f, 0.0f));
	}
}

void ATutorialBallisticEffectActor::ConfigureBulletTracer(const FVector& StartLocation, const FVector& EndLocation)
{
	const FVector TraceDelta = EndLocation - StartLocation;
	const float TraceLength = TraceDelta.Size();
	if (TraceLength <= KINDA_SMALL_NUMBER)
	{
		Destroy();
		return;
	}

	EffectType = ETutorialBallisticEffectType::BulletTracer;
	LifeSeconds = BulletTracerLifeSeconds;
	ElapsedSeconds = 0.0f;
	SetActorLocation(StartLocation);
	SetActorRotation(TraceDelta.Rotation());
	BuildTracerMesh(TraceLength, BulletTracerThickness, BulletTracerColor);
	InitialMeshScale = FVector::OneVector;

	if (FlashLight)
	{
		InitialLightIntensity = 0.0f;
		FlashLight->SetVisibility(false);
	}
}

void ATutorialBallisticEffectActor::ConfigureImpact(const FVector& ImpactLocation, const FVector& ImpactNormal, bool bBloodImpact)
{
	FVector Normal = ImpactNormal.GetSafeNormal();
	if (Normal.IsNearlyZero())
	{
		Normal = FVector::UpVector;
	}
	EffectType = bBloodImpact ? ETutorialBallisticEffectType::BloodImpact : ETutorialBallisticEffectType::SurfaceImpact;
	LifeSeconds = ImpactLifeSeconds;
	ElapsedSeconds = 0.0f;
	SetActorLocation(ImpactLocation + Normal * 1.5f);
	SetActorRotation(Normal.Rotation());
	BuildImpactMesh(
		bBloodImpact ? BloodImpactRadius : SurfaceImpactRadius,
		bBloodImpact ? BloodImpactColor : SurfaceImpactColor,
		bBloodImpact);
	InitialMeshScale = FVector::OneVector;

	if (FlashLight)
	{
		InitialLightIntensity = bBloodImpact ? 0.0f : 800.0f;
		FlashLight->SetVisibility(!bBloodImpact);
		FlashLight->SetIntensity(InitialLightIntensity);
		FlashLight->SetRelativeLocation(FVector(4.0f, 0.0f, 0.0f));
	}
}

void ATutorialBallisticEffectActor::ApplyEffectMaterial()
{
	if (EffectMesh && EffectMaterial)
	{
		EffectMesh->SetMaterial(0, EffectMaterial);
	}
}

void ATutorialBallisticEffectActor::BuildConeMesh(float Length, float Radius, const FLinearColor& Color)
{
	if (!EffectMesh)
	{
		return;
	}

	const int32 Segments = 10;
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FLinearColor> Colors;

	Vertices.Add(FVector(Length, 0.0f, 0.0f));
	Colors.Add(Color);

	for (int32 Index = 0; Index < Segments; ++Index)
	{
		const float Angle = 2.0f * PI * static_cast<float>(Index) / static_cast<float>(Segments);
		Vertices.Add(FVector(0.0f, FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius));
		Colors.Add(Color);
	}

	for (int32 Index = 0; Index < Segments; ++Index)
	{
		const int32 Current = Index + 1;
		const int32 Next = ((Index + 1) % Segments) + 1;
		Triangles.Add(0);
		Triangles.Add(Current);
		Triangles.Add(Next);
	}

	EffectMesh->ClearAllMeshSections();
	EffectMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, {}, {}, Colors, {}, false);
	ApplyEffectMaterial();
}

void ATutorialBallisticEffectActor::BuildTracerMesh(float Length, float Thickness, const FLinearColor& Color)
{
	if (!EffectMesh)
	{
		return;
	}

	const float HalfThickness = Thickness * 0.5f;
	TArray<FVector> Vertices = {
		FVector(0.0f, -HalfThickness, -HalfThickness),
		FVector(0.0f, HalfThickness, -HalfThickness),
		FVector(0.0f, HalfThickness, HalfThickness),
		FVector(0.0f, -HalfThickness, HalfThickness),
		FVector(Length, -HalfThickness, -HalfThickness),
		FVector(Length, HalfThickness, -HalfThickness),
		FVector(Length, HalfThickness, HalfThickness),
		FVector(Length, -HalfThickness, HalfThickness),
	};

	TArray<int32> Triangles = {
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		0, 4, 5, 0, 5, 1,
		1, 5, 6, 1, 6, 2,
		2, 6, 7, 2, 7, 3,
		3, 7, 4, 3, 4, 0,
	};

	TArray<FLinearColor> Colors;
	Colors.Init(Color, Vertices.Num());

	EffectMesh->ClearAllMeshSections();
	EffectMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, {}, {}, Colors, {}, false);
	ApplyEffectMaterial();
}

void ATutorialBallisticEffectActor::BuildImpactMesh(float Radius, const FLinearColor& Color, bool bBloodImpact)
{
	if (!EffectMesh)
	{
		return;
	}

	const int32 Segments = bBloodImpact ? 14 : 9;
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FLinearColor> Colors;

	Vertices.Add(FVector::ZeroVector);
	Colors.Add(Color);

	for (int32 Index = 0; Index < Segments; ++Index)
	{
		const float Angle = 2.0f * PI * static_cast<float>(Index) / static_cast<float>(Segments);
		const float RadiusScale = bBloodImpact
			? (Index % 3 == 0 ? 1.15f : (Index % 2 == 0 ? 0.62f : 0.9f))
			: (Index % 2 == 0 ? 1.0f : 0.45f);
		Vertices.Add(FVector(0.0f, FMath::Cos(Angle) * Radius * RadiusScale, FMath::Sin(Angle) * Radius * RadiusScale));
		Colors.Add(Color);
	}

	for (int32 Index = 0; Index < Segments; ++Index)
	{
		const int32 Current = Index + 1;
		const int32 Next = ((Index + 1) % Segments) + 1;
		Triangles.Add(0);
		Triangles.Add(Current);
		Triangles.Add(Next);
	}

	EffectMesh->ClearAllMeshSections();
	EffectMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, {}, {}, Colors, {}, false);
	ApplyEffectMaterial();
}
