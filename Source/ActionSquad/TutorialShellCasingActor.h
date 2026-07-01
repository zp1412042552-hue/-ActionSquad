#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialShellCasingActor.generated.h"

class UMaterialInterface;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialShellCasingActor : public AActor
{
	GENERATED_BODY()

public:
	ATutorialShellCasingActor();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Weapon FX")
	void LaunchShell(const FTransform& EjectionTransform, const FVector& LinearVelocity, const FVector& AngularVelocityDegrees);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UStaticMeshComponent> ShellMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX")
	TObjectPtr<UMaterialInterface> ShellMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX")
	FVector ShellMeshScale = FVector(0.018f, 0.018f, 0.04f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX", meta = (ClampMin = "0.1", Units = "s"))
	float ShellLifeSeconds = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon FX")
	FLinearColor ShellTint = FLinearColor(0.95f, 0.62f, 0.18f, 1.0f);
};
