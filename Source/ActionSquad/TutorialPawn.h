#pragma once

#include "CoreMinimal.h"
#include "ActionSquadTypes.h"
#include "GameFramework/Pawn.h"
#include "TutorialPawn.generated.h"

class ATutorialInstructionActor;
class ATutorialCommandAimVisualActor;
class ATutorialCommandMarkerActor;
class ATutorialTeamMemberActor;
class UCameraComponent;
class UCommandGestureComponent;
class UMotionControllerComponent;
class UOculusXRHandComponent;
class UCapsuleComponent;

UCLASS(Blueprintable)
class ACTIONSQUAD_API ATutorialPawn : public APawn
{
	GENERATED_BODY()

public:
	ATutorialPawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	void SelectTeam(ESelectedTeamTarget Target);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	void SpawnTutorialActors();

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Tutorial")
	bool CommandSelectedTeamToPointedLocation();

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
	ESelectedTeamTarget CurrentSelectedTeam = ESelectedTeamTarget::None;

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
	void ConfigureHandVisuals();

	FVector LastPreviewLocation = FVector::ZeroVector;
	TWeakObjectPtr<AActor> LastPreviewActor;
	float PreviewHoldSeconds = 0.0f;
	bool bHasContinuousFollowTarget = false;
	bool bCommandIssuedSinceSelection = false;
	bool bCanRearmSameTeamCommand = true;
};
