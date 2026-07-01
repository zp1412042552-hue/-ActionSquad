#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialCompletionZoneActor.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialCompletionZoneActor : public AActor
{
	GENERATED_BODY()

public:
	ATutorialCompletionZoneActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UBoxComponent> CompletionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	bool bOnlyCompleteAfterDoorBreached = true;

private:
	UFUNCTION()
	void HandleCompletionOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
