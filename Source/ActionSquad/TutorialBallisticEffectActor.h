#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialBallisticEffectActor.generated.h"

class UMaterialInterface;
class UPointLightComponent;
class UProceduralMeshComponent;

UENUM(BlueprintType)
enum class ETutorialBallisticEffectType : uint8
{
	MuzzleFlash,
	BulletTracer,
	BloodImpact,
	SurfaceImpact
};

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialBallisticEffectActor : public AActor
{
	GENERATED_BODY()

public:
	ATutorialBallisticEffectActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Weapon FX")
	void ConfigureMuzzleFlash(const FTransform& MuzzleTransform);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Weapon FX")
	void ConfigureBulletTracer(const FVector& StartLocation, const FVector& EndLocation);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Weapon FX")
	void ConfigureImpact(const FVector& ImpactLocation, const FVector& ImpactNormal, bool bBloodImpact);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UProceduralMeshComponent> EffectMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UPointLightComponent> FlashLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX")
	TObjectPtr<UMaterialInterface> EffectMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX", meta = (ClampMin = "0.01", Units = "s"))
	float MuzzleFlashLifeSeconds = 0.055f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX", meta = (ClampMin = "0.0", Units = "cm"))
	float MuzzleFlashLength = 42.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX", meta = (ClampMin = "0.0", Units = "cm"))
	float MuzzleFlashRadius = 13.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX")
	FLinearColor MuzzleFlashColor = FLinearColor(1.0f, 0.54f, 0.08f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX", meta = (ClampMin = "0.01", Units = "s"))
	float BulletTracerLifeSeconds = 0.045f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX", meta = (ClampMin = "0.1", Units = "cm"))
	float BulletTracerThickness = 1.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX")
	FLinearColor BulletTracerColor = FLinearColor(1.0f, 0.85f, 0.35f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX", meta = (ClampMin = "0.01", Units = "s"))
	float ImpactLifeSeconds = 0.28f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX", meta = (ClampMin = "0.0", Units = "cm"))
	float SurfaceImpactRadius = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX", meta = (ClampMin = "0.0", Units = "cm"))
	float BloodImpactRadius = 32.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX")
	FLinearColor SurfaceImpactColor = FLinearColor(1.0f, 0.62f, 0.14f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX")
	FLinearColor BloodImpactColor = FLinearColor(1.0f, 0.42f, 0.08f, 1.0f);

private:
	void ApplyEffectMaterial();
	void BuildConeMesh(float Length, float Radius, const FLinearColor& Color);
	void BuildTracerMesh(float Length, float Thickness, const FLinearColor& Color);
	void BuildImpactMesh(float Radius, const FLinearColor& Color, bool bBloodImpact);

	ETutorialBallisticEffectType EffectType = ETutorialBallisticEffectType::MuzzleFlash;
	float ElapsedSeconds = 0.0f;
	float LifeSeconds = 0.1f;
	float InitialLightIntensity = 0.0f;
	FVector InitialMeshScale = FVector::OneVector;
};
