#pragma once

#include "CoreMinimal.h"
#include "ActionSquadTypes.h"
#include "GameFramework/Actor.h"
#include "TutorialCommandMarkerActor.generated.h"

class USceneComponent;
class UTextRenderComponent;

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialCommandMarkerActor : public AActor
{
	GENERATED_BODY()

public:
	ATutorialCommandMarkerActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Command")
	void ShowMarker(ESelectedTeamTarget Target, const FVector& Location, const FVector& SurfaceNormal, float Progress);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Command")
	void HideMarker();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UTextRenderComponent> LabelText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (Units = "cm"))
	float RingRadius = 34.0f;

private:
	FVector MarkerNormal = FVector::UpVector;
	ESelectedTeamTarget DisplayTarget = ESelectedTeamTarget::None;
	float HoldProgress = 0.0f;
};
