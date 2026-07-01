#pragma once

#include "CoreMinimal.h"
#include "ActionSquadTypes.h"
#include "GameFramework/Actor.h"
#include "TutorialFloorMarkerActor.generated.h"

class ATutorialTeamMemberActor;
class USphereComponent;
class UTextRenderComponent;

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialFloorMarkerActor : public AActor
{
	GENERATED_BODY()

public:
	ATutorialFloorMarkerActor();

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	FVector GetCommandLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	bool IsTeamMemberAtMarker(const ATutorialTeamMemberActor* TeamMember) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UTextRenderComponent> MarkerLabel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USphereComponent> ReachedTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	ESelectedTeamTarget MarkerTarget = ESelectedTeamTarget::TeamA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial", meta = (ClampMin = "1.0", Units = "cm"))
	float ReachedRadius = 85.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	FVector CommandLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	FLinearColor TeamAColor = FLinearColor(0.0f, 0.85f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	FLinearColor TeamBColor = FLinearColor(1.0f, 0.55f, 0.06f, 1.0f);

private:
	void ApplyMarkerVisuals();
	void ApplyTriggerLayout();
};

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialFloorMarkerAActor : public ATutorialFloorMarkerActor
{
	GENERATED_BODY()

public:
	ATutorialFloorMarkerAActor();
};

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialFloorMarkerBActor : public ATutorialFloorMarkerActor
{
	GENERATED_BODY()

public:
	ATutorialFloorMarkerBActor();
};
