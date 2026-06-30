#include "TutorialCommandMarkerActor.h"

#include "Components/TextRenderComponent.h"
#include "DrawDebugHelpers.h"

ATutorialCommandMarkerActor::ATutorialCommandMarkerActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	LabelText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LabelText"));
	LabelText->SetupAttachment(SceneRoot);
	LabelText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	LabelText->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	LabelText->SetWorldSize(34.0f);
	LabelText->SetTextRenderColor(FColor::Cyan);
	LabelText->SetText(FText::FromString(TEXT("A")));

	SetActorHiddenInGame(true);
}

void ATutorialCommandMarkerActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsHidden())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FColor RingColor = bMarkerCanConfirm
		? (DisplayTarget == ESelectedTeamTarget::TeamB ? FColor(255, 180, 40) : FColor::Cyan)
		: FColor(230, 55, 55);
	const FVector Center = GetActorLocation() + MarkerNormal * 2.0f;
	const float InnerRadius = bMarkerCanConfirm ? FMath::Max(4.0f, RingRadius * FMath::Clamp(HoldProgress, 0.0f, 1.0f)) : 4.0f;
	const FVector MarkerRight = FVector::CrossProduct(MarkerNormal, MarkerForward).GetSafeNormal();

	DrawDebugCircle(World, Center, RingRadius, 64, RingColor, false, 0.0f, 0, 2.5f, MarkerForward, MarkerRight, false);
	DrawDebugCircle(World, Center, InnerRadius, 48, FColor::White, false, 0.0f, 0, 1.5f, MarkerForward, MarkerRight, false);
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
}
