#include "TutorialCompletionZoneActor.h"

#include "Components/BoxComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "TutorialInstructionActor.h"

ATutorialCompletionZoneActor::ATutorialCompletionZoneActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	CompletionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CompletionBox"));
	CompletionBox->SetupAttachment(SceneRoot);
	CompletionBox->SetBoxExtent(FVector(160.0f, 160.0f, 120.0f));
	CompletionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CompletionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CompletionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CompletionBox->SetGenerateOverlapEvents(true);
	CompletionBox->OnComponentBeginOverlap.AddDynamic(this, &ATutorialCompletionZoneActor::HandleCompletionOverlap);
}

void ATutorialCompletionZoneActor::HandleCompletionOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!Cast<APawn>(OtherActor))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (TActorIterator<ATutorialInstructionActor> It(World); It; ++It)
	{
		if (ATutorialInstructionActor* Instruction = *It)
		{
			Instruction->NotifyPlayerEnteredCompletionZone(this);
		}
	}
}
