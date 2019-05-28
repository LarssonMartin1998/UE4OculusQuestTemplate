#include "VRPawn.h"

#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Public/HeadMountedDisplayFunctionLibrary.h"

AVRPawn::AVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	if (CreateVRComponents())
	{
		BaseEyeHeight = 0.0f;
		bIsSeatedVR = true;
		SeatedHeightOffset = 75.0f;
	}

	if (CreateDefaultComponents())
	{
		CapsuleCollider->SetCapsuleSize(50.0f, 90.0f, false);
		CapsuleCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CapsuleCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		CapsuleCollider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	}
}

bool AVRPawn::CreateVRComponents()
{
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VR Root"));
	if (!VRRoot)
	{
		return false;
	}
	SetRootComponent(VRRoot);

	VRCameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VR Camera Root"));
	if (!VRCameraRoot)
	{
		return false;
	}
	VRCameraRoot->SetupAttachment(RootComponent);

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera"));
	if (!VRCamera)
	{
		return false;
	}
	VRCamera->SetupAttachment(VRCameraRoot);

	return true;
}

void AVRPawn::BeginPlay()
{
	Super::BeginPlay();

	SetupVR();
}

bool AVRPawn::CreateDefaultComponents()
{
	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	if (!CapsuleCollider)
	{
		return false;
	}
	CapsuleCollider->SetupAttachment(RootComponent);

	return true;
}

void AVRPawn::SetupVR()
{
	bool IsHMDConnected = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected();
	if (!IsHMDConnected)
	{
		FString ErrorMessage = TEXT("No HMD is connected! See AVRPawn::SetupVR");
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, ErrorMessage);
		}

		return;
	}
	// This can easily be expanded upon later if you want to support multiple platforms.
	const FName& HMDDeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();
	if (HMDDeviceName != FName("OculusHMD"))
	{
		FString ErrorMessage = TEXT("Cannot find an Oculus HMD! See AVRPawn::SetupVR");
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, ErrorMessage);
		}

		return;
	}

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	if (bIsSeatedVR)
	{
		VRCameraRoot->AddLocalOffset(FVector::UpVector * SeatedHeightOffset);
	}
}

void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}