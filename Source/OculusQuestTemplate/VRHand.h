#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRHand.generated.h"

class UMotionControllerComponent;
class USkeletalMeshComponent;

UCLASS()
class OCULUSQUESTTEMPLATE_API AVRHand : public AActor
{
	GENERATED_BODY()

public:

	AVRHand();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

protected:

	bool CreateVRComponents();

	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* MotionController;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* HandMesh;

public:
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsThumbUp() { return bIsThumbUp; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsPointing() { return bIsPointing; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetGripAxis() { return GripAxis; }

public:

	/* These are marked Public so that they can be set up for callbacks from VRPawn which has pointers to Left and Right hand.
	This way we don't have to run safety checks here for which hand it is when input is received. */
	UFUNCTION()
	void OnTeleportLPressed();
	UFUNCTION()
	void OnTeleportRPressed();

	UFUNCTION()
	void OnTeleportLReleased();
	UFUNCTION()
	void OnTeleportRReleased();

	UFUNCTION()
	void PointingLAxis(float Axis);
	UFUNCTION()
	void PointingRAxis(float Axis);

	UFUNCTION()
	void ThumbLAxis(float Axis);
	UFUNCTION()
	void ThumbRAxis(float Axis);

	UFUNCTION()
	void GripLAxis(float Axis);
	UFUNCTION()
	void GripRAxis(float Axis);

private:

	bool bIsThumbUp;

	bool bIsPointing;

	float GripAxis;

};