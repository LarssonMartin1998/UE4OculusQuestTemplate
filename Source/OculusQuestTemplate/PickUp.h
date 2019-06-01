#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUp.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class AVRHand;
class UMaterialInstanceDynamic;

UENUM(BlueprintType)
enum class EGripType : uint8
{
	GT_Default UMETA(DisplayName="Default"),
	GT_Pinch UMETA(DisplayName = "Pinch"),
	GT_PingPong UMETA(DisplayName = "PingPong")
};

UCLASS()
class OCULUSQUESTTEMPLATE_API APickUp : public AActor
{
	GENERATED_BODY()

public:

	APickUp();

protected:

	virtual void BeginPlay() override;

public:

	virtual void OnPickedUp(AVRHand* VRHand);

	virtual void OnReleased(AVRHand* VRHand);

	virtual void OnHoverSelected(AVRHand* VRHand);

	virtual void OnHoverDeselected(AVRHand* VRHand);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EGripType GetGripType() { return GripType; }

	FORCEINLINE float GetGripFlex() { return GripFlex; }

	UPROPERTY(EditAnywhere)
	bool bCanBePickedUp;

private:

	UPROPERTY(EditAnywhere)
	EGripType GripType;

	UPROPERTY(EditAnywhere)
	float GripFlex;

protected:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* AnchorRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UMaterialInstanceDynamic* StaticMeshMaterialInstance;
	float FresnelIntensityDefaultValue;
	int FresnelIntensityParamIndex;

	UPROPERTY(EditAnywhere)
	FName FresnelIntensityParamName;

	bool bIsCurrentlyPickedUp;

private:

	int HoverReferenceCount;
};