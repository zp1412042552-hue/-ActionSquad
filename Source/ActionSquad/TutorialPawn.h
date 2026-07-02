#pragma once

#include "CoreMinimal.h"
#include "ActionSquadTypes.h"
#include "GameFramework/Pawn.h"
#include "TutorialPawn.generated.h"

class ATutorialInstructionActor;
class ATutorialCommandAimVisualActor;
class ATutorialCommandMarkerActor;
class ATutorialBallisticEffectActor;
class ATutorialBulletMarkActor;
class ATutorialShellCasingActor;
class ATutorialTeamMemberActor;
class ATutorialWeaponActor;
class UCameraComponent;
class UChildActorComponent;
class UCommandGestureComponent;
class UMotionControllerComponent;
class UOculusXRHandComponent;
class UCapsuleComponent;
class USoundBase;

UENUM(BlueprintType)
enum class EGunPitchLocomotionState : uint8
{
	Stopped,
	MovingForward,
	MovingBackward
};

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialPawn : public APawn
{
	GENERATED_BODY()

public:
	ATutorialPawn();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	void SelectTeam(ESelectedTeamTarget Target);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	void SpawnTutorialActors();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	bool CommandSelectedTeamToPointedLocation();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Player Weapon")
	bool FirePlayerWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UCapsuleComponent> BodyCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> VROrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UMotionControllerComponent> LeftHandTrackingRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UMotionControllerComponent> RightHandTrackingRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UOculusXRHandComponent> LeftHandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> LeftHandGunAttachRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UChildActorComponent> PlayerWeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<USceneComponent> PlayerWeaponMuzzleReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UOculusXRHandComponent> RightHandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Components")
	TObjectPtr<UCommandGestureComponent> CommandGesture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	TSubclassOf<ATutorialTeamMemberActor> TeamMemberClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	TSubclassOf<ATutorialInstructionActor> TutorialInstructionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	FVector TeamAOffset = FVector(240.0f, -120.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Tutorial")
	FVector TeamBOffset = FVector(240.0f, 120.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon")
	TSubclassOf<ATutorialWeaponActor> PlayerWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|Firing")
	bool bEnablePlayerWeaponFire = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|Firing", meta = (ClampMin = "0.01", Units = "s"))
	float PlayerWeaponFireInterval = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|Firing", meta = (ClampMin = "0.0", Units = "cm"))
	float PlayerWeaponRange = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|Firing", meta = (ClampMin = "0.0"))
	float PlayerWeaponDamage = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|Firing")
	bool bUseWeaponActorMuzzleForFiring = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|Firing")
	bool bEnableHandTouchFireInput = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|Firing", meta = (ClampMin = "0.1", Units = "cm"))
	float HandTouchFireDistance = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|Firing", meta = (ClampMin = "0.1", Units = "cm"))
	float HandTouchFireReleaseDistance = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|Firing")
	bool bRequireHighConfidenceHandsForFire = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Player Weapon|Firing")
	float CurrentHandTouchFireDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	bool bSpawnPlayerWeaponMuzzleFlash = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	bool bSpawnPlayerWeaponBulletTracer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	bool bSpawnPlayerWeaponImpactEffect = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	bool bSpawnPlayerWeaponShellCasing = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	bool bSpawnPlayerWeaponBulletMarks = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	TSubclassOf<ATutorialBallisticEffectActor> MuzzleFlashEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	TSubclassOf<ATutorialBallisticEffectActor> BulletTracerEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	TSubclassOf<ATutorialBallisticEffectActor> ImpactEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	TSubclassOf<ATutorialShellCasingActor> ShellCasingClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	TSubclassOf<ATutorialBulletMarkActor> BulletMarkClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX", meta = (ClampMin = "0"))
	int32 MaxPlayerWeaponBulletMarks = 64;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	FVector ShellEjectionLocalVelocity = FVector(-35.0f, 115.0f, 65.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Weapon|FX")
	FVector ShellEjectionAngularVelocityDegrees = FVector(900.0f, 120.0f, 520.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> PlayerWeaponFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> PlayerWeaponSurfaceImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> PlayerWeaponCharacterImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> PlayerWeaponShellSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> CommandSelectASound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> CommandSelectBSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> CommandMoveIssuedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> CommandInvalidSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> LocomotionStartSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> LocomotionStopSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> HandTouchArmedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> HandTouchResetSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> BulletTracerWhizSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Audio")
	TObjectPtr<USoundBase> WeaponDryClickSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement")
	bool bEnableGunPitchLocomotion = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement", meta = (ClampMin = "0.0"))
	float GunForwardSpeed = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement", meta = (ClampMin = "0.0"))
	float GunBackwardSpeed = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement", meta = (ClampMin = "0.0", Units = "deg"))
	float GunForwardStartPitch = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement", meta = (ClampMin = "0.0", Units = "deg"))
	float GunForwardStopPitch = 14.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement", meta = (ClampMax = "0.0", Units = "deg"))
	float GunBackwardStartPitch = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement", meta = (ClampMax = "0.0", Units = "deg"))
	float GunBackwardStopPitch = -28.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement", meta = (ClampMin = "0.0"))
	float GunPitchSmoothingSpeed = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement", meta = (ClampMin = "0.0", Units = "s"))
	float GunLocomotionStartHoldSeconds = 0.12f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement", meta = (ClampMin = "0.0"))
	float GunLocomotionAccelerationSpeed = 4.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement", meta = (ClampMin = "0.0"))
	float GunLocomotionDecelerationSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Player Movement")
	bool bRequireLeftHandTrackingForGunLocomotion = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command")
	float CommandTraceDistance = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "0.0", Units = "s"))
	float CommandHoldSeconds = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command")
	bool bEnableContinuousPointFollow = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "0.0", Units = "s"))
	float ContinuousFollowRetargetInterval = 0.12f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "0.0", Units = "cm"))
	float ContinuousFollowRetargetDistance = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ContinuousFollowFingerExtendedMin = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ContinuousFollowSecondFingerCurledMax = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command")
	bool bDrawContinuousFollowAimLine = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "0.0", Units = "cm"))
	float CommandStableRadius = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WalkableCommandSurfaceMinZ = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command", meta = (ClampMin = "0.0", Units = "cm"))
	float CommandNavProjectionExtent = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command")
	TSubclassOf<ATutorialCommandMarkerActor> CommandMarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Command")
	TSubclassOf<ATutorialCommandAimVisualActor> CommandAimVisualClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Tutorial")
	TObjectPtr<ATutorialTeamMemberActor> TeamA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Tutorial")
	TObjectPtr<ATutorialTeamMemberActor> TeamB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Tutorial")
	TObjectPtr<ATutorialInstructionActor> TutorialInstruction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Tutorial")
	TObjectPtr<ATutorialCommandMarkerActor> CommandMarker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Tutorial")
	TObjectPtr<ATutorialCommandAimVisualActor> CommandAimVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Tutorial")
	TObjectPtr<ATutorialWeaponActor> PlayerWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Tutorial")
	ESelectedTeamTarget CurrentSelectedTeam = ESelectedTeamTarget::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Player Movement")
	EGunPitchLocomotionState GunLocomotionState = EGunPitchLocomotionState::Stopped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Player Movement")
	float RawGunPitchDegrees = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Player Movement")
	float SmoothedGunPitchDegrees = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Player Movement")
	float CurrentGunLocomotionSpeed = 0.0f;

private:
	UFUNCTION()
	void HandleCommandGestureRecognized(ECommandGesture Gesture);

	void UpdateCommandPreview(float DeltaSeconds);
	bool UpdateContinuousPointFollow(float DeltaSeconds, ESelectedTeamTarget MarkerTarget);
	bool IsContinuousFollowGestureHeld(ESelectedTeamTarget MarkerTarget) const;
	void StopSelectedTeamMovement(ESelectedTeamTarget MarkerTarget);
	void HideCommandVisuals();
	bool TraceCommandTarget(FHitResult& OutHit) const;
	bool GetCommandAimRay(FVector& OutStart, FVector& OutDirection) const;
	FVector GetCommandAimDirection() const;
	bool IsCommandHitConfirmable(const FHitResult& Hit) const;
	bool IsWalkableCommandHit(const FHitResult& Hit) const;
	bool ProjectCommandHitToNavigation(const FHitResult& Hit, FVector& OutProjectedLocation) const;
	bool IssueCommandAtHit(const FHitResult& Hit);
	ESelectedTeamTarget GetMarkerTarget() const;
	void TestSelectA();
	void TestSelectB();
	void TestMoveSelectedTeam();
	void ConfigurePlayerWeaponComponent();
	bool GetPlayerWeaponMuzzleTransform(FTransform& OutMuzzleTransform) const;
	FTransform GetPlayerWeaponShellEjectionTransform(const FTransform& MuzzleTransform) const;
	void SpawnPlayerWeaponMuzzleFlash(const FTransform& MuzzleTransform);
	void SpawnPlayerWeaponBulletTracer(const FVector& StartLocation, const FVector& EndLocation);
	void SpawnPlayerWeaponImpactEffect(const FHitResult& Hit, bool bBloodImpact);
	void SpawnPlayerWeaponShellCasing(const FTransform& MuzzleTransform);
	void SpawnPlayerWeaponBulletMark(const FHitResult& Hit, bool bCharacterMark);
	void PrunePlayerWeaponBulletMarks();
	void UpdateHandTouchFireInput(float DeltaSeconds);
	bool GetClosestHandBoneDistance(float& OutClosestDistance) const;
	bool HasValidHandsForTouchFire() const;
	bool GetHandBoneLocations(const UOculusXRHandComponent* HandMesh, TArray<FVector>& OutLocations) const;
	void UpdateGunPitchLocomotion(float DeltaSeconds);
	bool GetGunLocomotionPitch(float& OutPitchDegrees) const;
	bool HasValidLeftHandTrackingForGunLocomotion() const;
	FVector GetPlayerLocomotionDirection() const;
	void ConfigureHandVisuals();

	FVector LastPreviewLocation = FVector::ZeroVector;
	TWeakObjectPtr<AActor> LastPreviewActor;
	float PreviewHoldSeconds = 0.0f;
	float LastPlayerWeaponFireTime = -1000.0f;
	float GunLocomotionStartHoldTimer = 0.0f;
	TArray<TWeakObjectPtr<ATutorialBulletMarkActor>> PlayerWeaponBulletMarks;
	bool bHasContinuousFollowTarget = false;
	bool bHandTouchFireArmed = true;
	bool bCommandIssuedSinceSelection = false;
	bool bCanRearmSameTeamCommand = true;
};
