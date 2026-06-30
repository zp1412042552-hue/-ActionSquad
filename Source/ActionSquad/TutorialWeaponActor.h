#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialWeaponActor.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	ATutorialWeaponActor();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Weapon")
	FTransform GetMuzzleTransform() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> MuzzlePoint;
};
