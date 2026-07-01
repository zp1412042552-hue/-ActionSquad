#pragma once

#include "CoreMinimal.h"
#include "ActionSquadTypes.h"
#include "GameFramework/Actor.h"
#include "TutorialInstructionActor.generated.h"

class APlayerCameraManager;
class ATutorialCompletionZoneActor;
class ATutorialDoorActor;
class ATutorialFloorMarkerActor;
class ATutorialGestureDisplayActor;
class UTutorialCommandWidget;
class UWidgetComponent;

USTRUCT(BlueprintType)
struct FCommandTutorialStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial Step")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial Step", meta = (MultiLine = "true"))
	FText Body;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial Step")
	FText Highlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial Step")
	FText FooterHint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial Step")
	ECommandGesture RequiredGesture = ECommandGesture::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial Step")
	int32 GestureDisplayIndex = INDEX_NONE;
};

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialInstructionActor : public AActor
{
	GENERATED_BODY()

public:
	ATutorialInstructionActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	void StartTutorial();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	void NotifyGesture(ECommandGesture Gesture);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	void NotifyPlayerFiredWeapon();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	void NotifyDoorBreached(ATutorialDoorActor* Door);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	void NotifyPlayerEnteredCompletionZone(ATutorialCompletionZoneActor* Zone);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	void SetCurrentStep(int32 StepIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UWidgetComponent> TutorialWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	TObjectPtr<ATutorialGestureDisplayActor> GestureDisplayActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	bool bAutoSpawnGestureDisplayActor = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	TArray<FCommandTutorialStep> Steps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	TObjectPtr<ATutorialFloorMarkerActor> TeamAMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	TObjectPtr<ATutorialFloorMarkerActor> TeamBMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	TObjectPtr<ATutorialDoorActor> BreachDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	TObjectPtr<ATutorialCompletionZoneActor> CompletionZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial", meta = (ClampMin = "1.0", Units = "cm"))
	float LocomotionTutorialForwardDistance = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Layout")
	FVector2D WidgetDrawSize = FVector2D(920.0f, 340.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Layout", meta = (ClampMin = "0.01"))
	float ScreenScale = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Layout", meta = (Units = "cm"))
	float FollowDistance = 58.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Layout", meta = (Units = "cm"))
	float VerticalOffset = -4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Layout")
	float LocationLerpSpeed = 7.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Layout")
	float RotationLerpSpeed = 8.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Runtime")
	int32 CurrentStepIndex = INDEX_NONE;

private:
	void BuildDefaultSteps();
	void RemoveDuplicateInstructionActors();
	void InitializeWidget();
	void ApplyScreenLayout();
	void ApplyCurrentStepToWidget();
	void ResolveGestureDisplayActor();
	void ResolveTutorialTargets();
	void UpdateObjectiveProgress();
	void CompleteTutorial();
	void UpdateTransform(float DeltaSeconds);
	void UpdateGestureDisplayTransform();
	bool EnsureCamera();
	bool IsStep(int32 StepIndex) const;

	UPROPERTY()
	TObjectPtr<UTutorialCommandWidget> CachedWidget;

	UPROPERTY()
	TObjectPtr<APlayerCameraManager> PlayerCameraManager;

	FVector StepStartPlayerLocation = FVector::ZeroVector;
	bool bHasInitialTransformLock = false;
	bool bHasStepStartPlayerLocation = false;
};
