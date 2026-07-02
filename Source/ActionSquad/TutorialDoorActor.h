#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialDoorActor.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;
class UBoxComponent;
class USoundBase;

UENUM(BlueprintType)
enum class ETutorialDoorState : uint8
{
	Closed UMETA(DisplayName = "Closed"),
	Targeted UMETA(DisplayName = "Targeted"),
	Breached UMETA(DisplayName = "Breached")
};

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialDoorActor : public AActor
{
	GENERATED_BODY()

public:
	ATutorialDoorActor();

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Door")
	void SetDoorState(ETutorialDoorState NewState);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Door")
	void BreachFrom(const FVector& BreacherLocation);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Door")
	FVector GetBreachStandLocation(const FVector& BreacherLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Door")
	bool IsBreacherInsideBreachTrigger(const AActor* Breacher) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UTextRenderComponent> DoorLabel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> FrontBreachStandPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> BackBreachStandPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UBoxComponent> BreachTriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Door")
	ETutorialDoorState DoorState = ETutorialDoorState::Closed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Door")
	float BreachImpulse = 85000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Door")
	float BreachUpImpulse = 12000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Door")
	float BreachStandDistance = 115.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Door")
	FVector BreachTriggerExtent = FVector(150.0f, 190.0f, 130.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> BreachSound;

private:
	void ApplyBreachPointLayout();
};
