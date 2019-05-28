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

};