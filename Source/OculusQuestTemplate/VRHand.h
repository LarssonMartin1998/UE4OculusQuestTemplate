#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUp.h"
#include "Math/Vector.h"
#include "VRHand.generated.h"

class UMotionControllerComponent;
class USkeletalMeshComponent;
class UBoxComponent;

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

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollider;

public:
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsThumbUp() { return bIsThumbUp; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsPointing() { return bIsPointing; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetGripAxis() { return CurrentPickUp ? CurrentPickUp->GetGripFlex() : GripAxis; }

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

public:

	UFUNCTION(BlueprintCallable)
	FORCEINLINE APickUp* GetCurrentPickUp() { return CurrentPickUp; }

private:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void InternalHoverSelectNewPickUp(APickUp* PickUp);

	void InternalHoverDeselectClosestPickUp();

	APickUp* CurrentPickUp;

	APickUp* ClosestPickUp;

	TArray<APickUp*> OverlappedPickUps;

};