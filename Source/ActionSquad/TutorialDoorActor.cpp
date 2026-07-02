#include "TutorialDoorActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "TutorialInstructionActor.h"
#include "UObject/ConstructorHelpers.h"

ATutorialDoorActor::ATutorialDoorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(SceneRoot);
	DoorMesh->SetMobility(EComponentMobility::Movable);
	DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));
	DoorMesh->ComponentTags.Add(TEXT("Door"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		DoorMesh->SetStaticMesh(CubeMesh.Object);
		DoorMesh->SetRelativeScale3D(FVector(0.12f, 1.0f, 2.1f));
		DoorMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 105.0f));
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> BreachSoundAsset(
		TEXT("/Game/Audio/Tutorial/AS_SFX_Door_Breach_Kick.AS_SFX_Door_Breach_Kick"));
	BreachSound = BreachSoundAsset.Object;

	DoorLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DoorLabel"));
	DoorLabel->SetupAttachment(SceneRoot);
	DoorLabel->SetRelativeLocation(FVector(0.0f, -70.0f, 235.0f));
	DoorLabel->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	DoorLabel->SetText(FText::FromString(TEXT("DOOR")));
	DoorLabel->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	DoorLabel->SetWorldSize(28.0f);
	DoorLabel->SetTextRenderColor(FColor(220, 235, 245));

	FrontBreachStandPoint = CreateDefaultSubobject<USceneComponent>(TEXT("FrontBreachStandPoint"));
	FrontBreachStandPoint->SetupAttachment(SceneRoot);
	FrontBreachStandPoint->SetRelativeLocation(FVector(-BreachStandDistance, 0.0f, 0.0f));

	BackBreachStandPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BackBreachStandPoint"));
	BackBreachStandPoint->SetupAttachment(SceneRoot);
	
	BreachTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BreachTriggerBox"));
	BreachTriggerBox->SetupAttachment(SceneRoot);
	BreachTriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BreachTriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	BreachTriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BreachTriggerBox->SetGenerateOverlapEvents(true);
	BreachTriggerBox->SetHiddenInGame(true);

	ApplyBreachPointLayout();
}

void ATutorialDoorActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyBreachPointLayout();
}

void ATutorialDoorActor::SetDoorState(ETutorialDoorState NewState)
{
	DoorState = NewState;

	if (!DoorLabel)
	{
		return;
	}

	switch (DoorState)
	{
	case ETutorialDoorState::Closed:
		DoorLabel->SetText(FText::FromString(TEXT("LOCKED")));
		DoorLabel->SetTextRenderColor(FColor(235, 60, 60));
		break;
	case ETutorialDoorState::Targeted:
		DoorLabel->SetText(FText::FromString(TEXT("BREACH")));
		DoorLabel->SetTextRenderColor(FColor(255, 190, 55));
		break;
	case ETutorialDoorState::Breached:
		DoorLabel->SetText(FText::FromString(TEXT("OPEN")));
		DoorLabel->SetTextRenderColor(FColor(75, 235, 105));
		break;
	default:
		break;
	}
}

void ATutorialDoorActor::BreachFrom(const FVector& BreacherLocation)
{
	if (DoorState == ETutorialDoorState::Breached)
	{
		return;
	}

	SetDoorState(ETutorialDoorState::Breached);
	if (BreachSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BreachSound, GetActorLocation());
	}

	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<ATutorialInstructionActor> It(World); It; ++It)
		{
			if (ATutorialInstructionActor* Instruction = *It)
			{
				Instruction->NotifyDoorBreached(this);
			}
		}
	}

	if (!DoorMesh)
	{
		return;
	}

	DoorMesh->SetSimulatePhysics(true);
	DoorMesh->SetCollisionProfileName(TEXT("PhysicsActor"));

	FVector Direction = DoorMesh->GetComponentLocation() - BreacherLocation;
	Direction.Z = 0.0f;
	if (!Direction.Normalize())
	{
		Direction = GetActorForwardVector();
	}

	const FVector Impulse = Direction * BreachImpulse + FVector(0.0f, 0.0f, BreachUpImpulse);
	DoorMesh->AddImpulse(Impulse, NAME_None, true);
}

FVector ATutorialDoorActor::GetBreachStandLocation(const FVector& BreacherLocation) const
{
	const FVector FrontLocation = FrontBreachStandPoint ? FrontBreachStandPoint->GetComponentLocation() : GetActorLocation() - GetActorForwardVector() * BreachStandDistance;
	const FVector BackLocation = BackBreachStandPoint ? BackBreachStandPoint->GetComponentLocation() : GetActorLocation() + GetActorForwardVector() * BreachStandDistance;

	return FVector::DistSquared2D(BreacherLocation, BackLocation) < FVector::DistSquared2D(BreacherLocation, FrontLocation)
		? BackLocation
		: FrontLocation;
}

bool ATutorialDoorActor::IsBreacherInsideBreachTrigger(const AActor* Breacher) const
{
	return Breacher && BreachTriggerBox && BreachTriggerBox->IsOverlappingActor(Breacher);
}

void ATutorialDoorActor::ApplyBreachPointLayout()
{
	if (FrontBreachStandPoint)
	{
		FrontBreachStandPoint->SetRelativeLocation(FVector(-BreachStandDistance, 0.0f, 0.0f));
	}

	if (BackBreachStandPoint)
	{
		BackBreachStandPoint->SetRelativeLocation(FVector(BreachStandDistance, 0.0f, 0.0f));
	}

	if (BreachTriggerBox)
	{
		BreachTriggerBox->SetBoxExtent(BreachTriggerExtent);
		BreachTriggerBox->SetRelativeLocation(FVector::ZeroVector);
	}
}
