#include "TutorialFloorMarkerActor.h"

#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "TutorialTeamMemberActor.h"

ATutorialFloorMarkerActor::ATutorialFloorMarkerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	MarkerLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("MarkerLabel"));
	MarkerLabel->SetupAttachment(SceneRoot);
	MarkerLabel->SetRelativeLocation(FVector(0.0f, 0.0f, 4.0f));
	MarkerLabel->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	MarkerLabel->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	MarkerLabel->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	MarkerLabel->SetWorldSize(95.0f);
	MarkerLabel->SetText(FText::FromString(TEXT("A")));

	ReachedTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("ReachedTrigger"));
	ReachedTrigger->SetupAttachment(SceneRoot);
	ReachedTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ReachedTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	ReachedTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ReachedTrigger->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	ReachedTrigger->SetGenerateOverlapEvents(true);
	ReachedTrigger->SetHiddenInGame(true);
	ReachedTrigger->ShapeColor = FColor(80, 220, 255);

	ApplyMarkerVisuals();
	ApplyTriggerLayout();
}

void ATutorialFloorMarkerActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyMarkerVisuals();
	ApplyTriggerLayout();
}

FVector ATutorialFloorMarkerActor::GetCommandLocation() const
{
	return GetActorLocation() + GetActorQuat().RotateVector(CommandLocationOffset);
}

bool ATutorialFloorMarkerActor::IsTeamMemberAtMarker(const ATutorialTeamMemberActor* TeamMember) const
{
	if (!TeamMember)
	{
		return false;
	}

	if (ReachedTrigger && ReachedTrigger->IsOverlappingActor(TeamMember))
	{
		return true;
	}

	return FVector::DistSquared2D(TeamMember->GetActorLocation(), GetCommandLocation()) <= FMath::Square(ReachedRadius);
}

void ATutorialFloorMarkerActor::ApplyMarkerVisuals()
{
	const bool bTeamB = MarkerTarget == ESelectedTeamTarget::TeamB;
	const FLinearColor MarkerColor = bTeamB ? TeamBColor : TeamAColor;

	if (MarkerLabel)
	{
		MarkerLabel->SetText(FText::FromString(bTeamB ? TEXT("B") : TEXT("A")));
		MarkerLabel->SetTextRenderColor(MarkerColor.ToFColor(true));
	}

}

void ATutorialFloorMarkerActor::ApplyTriggerLayout()
{
	if (!ReachedTrigger)
	{
		return;
	}

	ReachedTrigger->SetSphereRadius(FMath::Max(1.0f, ReachedRadius));
	ReachedTrigger->SetRelativeLocation(CommandLocationOffset);
	ReachedTrigger->ShapeColor = MarkerTarget == ESelectedTeamTarget::TeamB
		? FColor(255, 145, 20)
		: FColor(0, 215, 255);
}

ATutorialFloorMarkerAActor::ATutorialFloorMarkerAActor()
{
	MarkerTarget = ESelectedTeamTarget::TeamA;
}

ATutorialFloorMarkerBActor::ATutorialFloorMarkerBActor()
{
	MarkerTarget = ESelectedTeamTarget::TeamB;
}
