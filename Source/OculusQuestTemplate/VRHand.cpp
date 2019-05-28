#include "VRHand.h"
#include "Public/MotionControllerComponent.h"
#include "Components/SkeletalMeshComponent.h"

AVRHand::AVRHand()
{
	PrimaryActorTick.bCanEverTick = true;

	CreateVRComponents();
}

bool AVRHand::CreateVRComponents()
{
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>("MotionController");
	if (!MotionController)
	{
		return false;
	}
	SetRootComponent(MotionController);

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Hand Mesh");
	if (!HandMesh)
	{
		return false;
	}
	HandMesh->SetupAttachment(RootComponent);

	return true;
}

void AVRHand::BeginPlay()
{
	Super::BeginPlay();
}

void AVRHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}