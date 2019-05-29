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

void AVRHand::OnTeleportLPressed()
{

}

void AVRHand::OnTeleportRPressed()
{

}

void AVRHand::OnTeleportLReleased()
{

}

void AVRHand::OnTeleportRReleased()
{

}

void AVRHand::PointingLAxis(float Axis)
{
	bIsPointing = Axis > 0.5f;
}

void AVRHand::PointingRAxis(float Axis)
{
	bIsPointing = Axis > 0.5f;
}

void AVRHand::ThumbLAxis(float Axis)
{
	bIsThumbUp = Axis > 0.5f;
}

void AVRHand::ThumbRAxis(float Axis)
{
	bIsThumbUp = Axis > 0.5f;
}

void AVRHand::GripLAxis(float Axis)
{
	GripAxis = Axis;
}

void AVRHand::GripRAxis(float Axis)
{
	GripAxis = Axis;
}