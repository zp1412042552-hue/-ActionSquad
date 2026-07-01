#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialBulletMarkActor.generated.h"

class UMaterialInterface;
class UPrimitiveComponent;
class UProceduralMeshComponent;

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialBulletMarkActor : public AActor
{
	GENERATED_BODY()

public:
	ATutorialBulletMarkActor();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Weapon FX")
	void ConfigureBulletMark(
		const FVector& ImpactLocation,
		const FVector& ImpactNormal,
		UPrimitiveComponent* HitComponent,
		bool bCharacterMark);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UProceduralMeshComponent> MarkMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Bullet Mark")
	TObjectPtr<UMaterialInterface> MarkMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Bullet Mark", meta = (ClampMin = "0.1", Units = "s"))
	float SurfaceLifeSeconds = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Bullet Mark", meta = (ClampMin = "0.1", Units = "s"))
	float CharacterLifeSeconds = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Bullet Mark", meta = (ClampMin = "0.1", Units = "cm"))
	float SurfaceRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Bullet Mark", meta = (ClampMin = "0.1", Units = "cm"))
	float CharacterRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Bullet Mark", meta = (ClampMin = "0.0", Units = "cm"))
	float SurfaceOffset = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Bullet Mark", meta = (ClampMin = "0.0", Units = "cm"))
	float CharacterOffset = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Bullet Mark")
	FLinearColor SurfaceCoreColor = FLinearColor(0.025f, 0.023f, 0.021f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Bullet Mark")
	FLinearColor SurfaceEdgeColor = FLinearColor(0.18f, 0.17f, 0.16f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Bullet Mark")
	FLinearColor CharacterCoreColor = FLinearColor(1.0f, 0.42f, 0.08f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Bullet Mark")
	FLinearColor CharacterEdgeColor = FLinearColor(1.0f, 0.70f, 0.20f, 1.0f);

private:
	void BuildMarkMesh(float Radius, const FLinearColor& CoreColor, const FLinearColor& EdgeColor, int32 Seed);
	void ApplyMarkMaterial();
};
