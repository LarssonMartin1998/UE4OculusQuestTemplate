#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

class USceneComponent;
class UCameraComponent;
class UCapsuleComponent;

UCLASS()
class OCULUSQUESTTEMPLATE_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:

	AVRPawn();

protected:

	virtual void BeginPlay() override;

	void SetupVR();

public:	

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	FORCEINLINE bool CreateVRComponents();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* VRCameraRoot;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* VRCamera;

protected:

	FORCEINLINE bool CreateDefaultComponents();

	/* A trigger which will be used for any collision gameplay events, not used for physical movement */
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CapsuleCollider;

private:

	UPROPERTY(EditAnywhere)
	bool bIsSeatedVR;

	UPROPERTY(EditAnywhere)
	float SeatedHeightOffset;
};