#pragma once

#include "CoreMinimal.h"
#include "ActionSquadTypes.h"
#include "GameFramework/Actor.h"
#include "TutorialCommandMarkerActor.generated.h"

class USceneComponent;
class UTextRenderComponent;
class UMaterialInterface;
class UProceduralMeshComponent;

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialCommandMarkerActor : public AActor
{
	GENERATED_BODY()

public:
	ATutorialCommandMarkerActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Command")
	void ShowMarker(ESelectedTeamTarget Target, const FVector& Location, const FVector& SurfaceNormal, const FVector& AimDirection, float Progress, bool bCanConfirm);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Command")
	void HideMarker();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UTextRenderComponent> LabelText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UProceduralMeshComponent> RingMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UProceduralMeshComponent> FillMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (Units = "cm"))
	float RingRadius = 34.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "0.5", Units = "cm"))
	float RingThickness = 4.0f;

private:
	void RebuildMarkerMeshes();
	void BuildRingMesh(const FLinearColor& Color);
	void BuildFillMesh(float Radius, const FLinearColor& Color);
	FLinearColor ResolveMarkerColor() const;

	FVector MarkerNormal = FVector::UpVector;
	FVector MarkerForward = FVector::ForwardVector;
	ESelectedTeamTarget DisplayTarget = ESelectedTeamTarget::None;
	float HoldProgress = 0.0f;
	bool bMarkerCanConfirm = false;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> VertexColorMaterial;
};
