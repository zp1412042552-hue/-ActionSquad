#pragma once

#include "CoreMinimal.h"
#include "ActionSquadTypes.h"
#include "GameFramework/Actor.h"
#include "TutorialCommandAimVisualActor.generated.h"

class UMaterialInterface;
class UProceduralMeshComponent;
class USceneComponent;

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialCommandAimVisualActor : public AActor
{
	GENERATED_BODY()

public:
	ATutorialCommandAimVisualActor();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Command")
	void ShowAim(ESelectedTeamTarget Target, const FVector& Start, const FVector& End, bool bValidTarget);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Command")
	void HideAim();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UProceduralMeshComponent> BeamMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UProceduralMeshComponent> ImpactMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "0.1", Units = "cm"))
	float BeamRadius = 1.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "1.0", Units = "cm"))
	float ImpactRadius = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "6", ClampMax = "32"))
	int32 SegmentCount = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command")
	FLinearColor TeamAColor = FLinearColor(0.0f, 0.86f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command")
	FLinearColor TeamBColor = FLinearColor(1.0f, 0.58f, 0.08f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command")
	FLinearColor InvalidColor = FLinearColor(1.0f, 0.08f, 0.05f, 1.0f);

private:
	void BuildBeamMesh(float Length, const FLinearColor& Color);
	void BuildImpactMesh(const FLinearColor& Color);
	FLinearColor ResolveColor(ESelectedTeamTarget Target, bool bValidTarget) const;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> VertexColorMaterial;
};
