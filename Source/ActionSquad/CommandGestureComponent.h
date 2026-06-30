#pragma once

#include "CoreMinimal.h"
#include "ActionSquadTypes.h"
#include "Components/ActorComponent.h"
#include "HandPose.h"
#include "CommandGestureComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCommandGestureRecognized, ECommandGesture, Gesture);

USTRUCT(BlueprintType)
struct FFingerExtensionPose
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Finger Pose", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Thumb = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Finger Pose", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Index = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Finger Pose", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Middle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Finger Pose", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Ring = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Finger Pose", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Pinky = 0.0f;
};

UCLASS(ClassGroup = (ActionSquad), meta = (BlueprintSpawnableComponent))
class ACTIONSQUAD_API UCommandGestureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCommandGestureComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Gesture")
	void SubmitFingerExtensionPose(const FFingerExtensionPose& Pose);

	UFUNCTION(BlueprintCallable, Category = "Action Squad|Gesture")
	void ForceRecognizeGesture(ECommandGesture Gesture);

	bool GetCurrentHandPose(FHandPose& OutHandPose, FFingerExtensionPose* OutFingerPose = nullptr) const;

	UPROPERTY(BlueprintAssignable, Category = "Action Squad|Gesture")
	FOnCommandGestureRecognized OnCommandGestureRecognized;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Gesture")
	bool bEnableMetaHandPolling = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Gesture", meta = (ClampMin = "0.0", Units = "s"))
	float RequiredHoldSeconds = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Gesture")
	ECommandGesture StableGesture = ECommandGesture::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Gesture")
	float StableGestureSeconds = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Gesture")
	FFingerExtensionPose LastFingerPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Gesture", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RecordedHandPoseConfidenceFloor = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Gesture", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RecordedFingerPoseConfidenceFloor = 0.72f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Gesture", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RecordedGestureConfidenceMargin = 0.05f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Gesture")
	float LastRecordedHandPoseConfidence = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Gesture")
	float LastRecordedHandPoseError = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Gesture")
	float LastRecordedFingerPoseConfidence = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Recognition Zone")
	bool bUseRecognitionZone = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Recognition Zone")
	bool bDrawRecognitionZone = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Recognition Zone", meta = (Units = "cm"))
	FVector RecognitionZoneCenter = FVector(17.5f, 0.0f, 2.5f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Squad|Recognition Zone", meta = (Units = "cm"))
	FVector RecognitionZoneExtent = FVector(17.5f, 35.0f, 22.5f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action Squad|Recognition Zone")
	bool bHandInsideRecognitionZone = false;

private:
	void LoadRecordedGestureProfile();
	void UpdateCandidate(ECommandGesture Candidate, float DeltaTime);
	ECommandGesture ClassifyHandPose(const FHandPose& Pose, const FFingerExtensionPose& FingerPose, float* OutBestConfidence = nullptr, float* OutBestError = nullptr, float* OutBestFingerConfidence = nullptr) const;
	bool PollMetaHandPose(FHandPose& OutHandPose, FFingerExtensionPose* OutFingerPose) const;
	bool GetRecognizerHandLocation(FVector& OutHandLocation) const;
	bool IsHandInsideRecognitionZone(const FVector& HandLocation) const;
	void DrawRecognitionZone(ECommandGesture Candidate) const;
	FRotator GetRecognizerWristRotator() const;
	static float Clamp01(float Value);
	static float ComputeFingerPoseConfidence(const FFingerExtensionPose& ReferencePose, const FFingerExtensionPose& Pose);

	FHandPose RecordedSelectAHandPose;
	FHandPose RecordedSelectBHandPose;
	FFingerExtensionPose RecordedSelectAFingerPose;
	FFingerExtensionPose RecordedSelectBFingerPose;
	bool bHasRecordedSelectAHandPose = false;
	bool bHasRecordedSelectBHandPose = false;
	bool bHasRecordedSelectAFingerPose = false;
	bool bHasRecordedSelectBFingerPose = false;
	bool bStableGestureBroadcasted = false;
};
