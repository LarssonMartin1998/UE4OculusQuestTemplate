#include "VRPawn.h"

AVRPawn::AVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}