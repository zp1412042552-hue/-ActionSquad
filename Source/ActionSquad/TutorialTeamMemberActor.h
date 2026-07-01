#pragma once

#include "CoreMinimal.h"
#include "ActionSquadTypes.h"
#include "GameFramework/Character.h"
#include "TutorialTeamMemberActor.generated.h"

class UAnimSequence;
class USkeletalMeshComponent;
class ATutorialDoorActor;
class ATutorialBallisticEffectActor;
class ATutorialBulletMarkActor;
class ATutorialWeaponActor;
class UWidgetComponent;
class UTeamNameplateWidget;

USTRUCT(BlueprintType)
struct FTeamMemberAnimationSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TObjectPtr<UAnimSequence> RelaxedIdle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TObjectPtr<UAnimSequence> AlertIdle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TObjectPtr<UAnimSequence> CrouchAlert = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TObjectPtr<UAnimSequence> Walk = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TObjectPtr<UAnimSequence> Run = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TObjectPtr<UAnimSequence> CrouchWalk = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TObjectPtr<UAnimSequence> Fire = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TObjectPtr<UAnimSequence> Reload = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TObjectPtr<UAnimSequence> HitReact = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TObjectPtr<UAnimSequence> Death = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TObjectPtr<UAnimSequence> BreachKick = nullptr;
};

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialTeamMemberActor : public ACharacter
{
	GENERATED_BODY()

public:
	ATutorialTeamMemberActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Team")
	void InitializeTeamMember(ETeamMemberRole InRole);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Team")
	void SetSelected(bool bInSelected);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Team")
	void PlayTeamAnimation(ETeamMemberAnimState NewState);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Team")
	void MoveToCommandLocation(const FVector& WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Team")
	void StopCommandMovement();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Team")
	void SnapToGround();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Team")
	void BreachDoor(ATutorialDoorActor* Door);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Weapon")
	bool FireWeaponForward();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Weapon")
	bool FireWeaponAtLocation(const FVector& TargetLocation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USkeletalMeshComponent> SoldierMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UWidgetComponent> NameplateWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<ATutorialWeaponActor> EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Team")
	ETeamMemberRole TeamRole = ETeamMemberRole::TeamA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Team")
	bool bSelected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Combat")
	bool bCanReceivePlayerWeaponDamage = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Combat", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Combat")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Combat")
	bool bDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Team")
	float NameplateHeight = 210.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Movement")
	bool bSnapToGroundOnConstruction = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Movement")
	float GroundTraceUp = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Movement")
	float GroundTraceDown = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Movement")
	float MovementSpeed = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Movement")
	float AcceptanceRadius = 65.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Movement", meta = (ClampMin = "0.0", Units = "s"))
	float LowSpeedStopSeconds = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Movement")
	float NavProjectionExtent = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Movement")
	float StepUpHeight = 38.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Door")
	float DoorBreachDistance = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon")
	bool bSpawnWeapon = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon")
	TSubclassOf<ATutorialWeaponActor> WeaponActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon")
	FName WeaponSocketName = TEXT("Gun");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon")
	FName FallbackWeaponBoneName = TEXT("RightHand");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|Firing")
	bool bEnableWeaponFire = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|Firing", meta = (ClampMin = "0.01", Units = "s"))
	float WeaponFireInterval = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|Firing", meta = (ClampMin = "0.0", Units = "cm"))
	float WeaponRange = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|Firing", meta = (ClampMin = "0.0"))
	float WeaponDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|FX")
	bool bSpawnWeaponMuzzleFlash = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|FX")
	bool bSpawnWeaponBulletTracer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|FX")
	bool bSpawnWeaponImpactEffect = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|FX")
	bool bSpawnWeaponBulletMarks = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|FX")
	TSubclassOf<ATutorialBallisticEffectActor> MuzzleFlashEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|FX")
	TSubclassOf<ATutorialBallisticEffectActor> BulletTracerEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|FX")
	TSubclassOf<ATutorialBallisticEffectActor> ImpactEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|FX")
	TSubclassOf<ATutorialBulletMarkActor> BulletMarkClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Weapon|FX", meta = (ClampMin = "0"))
	int32 MaxWeaponBulletMarks = 64;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Animations")
	FTeamMemberAnimationSet AnimationSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Animations")
	ETeamMemberAnimState CurrentAnimState = ETeamMemberAnimState::RelaxedIdle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Animations")
	TObjectPtr<UAnimSequence> CurrentAnimation = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Movement")
	bool bHasMoveTarget = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Movement")
	FVector MoveTargetLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Door")
	TObjectPtr<ATutorialDoorActor> PendingBreachDoor;

private:
	void StartMoveToLocation(const FVector& WorldLocation, bool bClearPendingDoor);
	void LoadDefaultAssets();
	void RefreshNameplate();
	void SpawnAndAttachWeapon();
	FName ResolveWeaponAttachSocket() const;
	bool FireWeaponInDirection(const FVector& ShotDirection);
	bool GetWeaponMuzzleTransform(FTransform& OutMuzzleTransform) const;
	FTransform GetCorrectedWeaponMuzzleTransform(const FTransform& MuzzleTransform) const;
	void SpawnWeaponMuzzleFlash(const FTransform& MuzzleTransform);
	void SpawnWeaponBulletTracer(const FVector& StartLocation, const FVector& EndLocation);
	void SpawnWeaponImpactEffect(const FHitResult& Hit, bool bCharacterImpact);
	void SpawnWeaponBulletMark(const FHitResult& Hit, bool bCharacterMark);
	void PruneWeaponBulletMarks();
	UAnimSequence* ResolveAnimation(ETeamMemberAnimState State) const;
	void FinishMoveCommand();
	void UpdateCommandMovement(float DeltaSeconds);
	void UpdateMovementAnimation();
	void ResumeAfterHitReaction();

	float LowSpeedMoveSeconds = 0.0f;
	float LastWeaponFireTime = -1000.0f;
	TArray<TWeakObjectPtr<ATutorialBulletMarkActor>> WeaponBulletMarks;
	FTimerHandle HitReactionTimerHandle;
};

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialTeamAActor : public ATutorialTeamMemberActor
{
	GENERATED_BODY()

public:
	ATutorialTeamAActor();
};

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialTeamBActor : public ATutorialTeamMemberActor
{
	GENERATED_BODY()

public:
	ATutorialTeamBActor();
};
