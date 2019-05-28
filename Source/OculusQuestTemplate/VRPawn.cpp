#include "VRPawn.h"

#include "Public/HeadMountedDisplayFunctionLibrary.h"
#include "Engine/World.h"
#include "Math/TransformNonVectorized.h"
#include "Math/Vector.h"
#include "Engine/EngineTypes.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

AVRPawn::AVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseEyeHeight = 0.0f;
	AutoPossessAI = EAutoPossessAI::Disabled;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AIControllerClass = nullptr;

	bIsSeatedVR = true;
	SeatedHeightOffset = 75.0f;

	if (CreateVRComponents())
	{
		VRCamera->SetProjectionMode(ECameraProjectionMode::Perspective);
		VRCamera->SetFieldOfView(90.0f);
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

	VRHandLeftRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VR Hand Left Root"));
	if (!VRHandLeftRoot)
	{
		return false;
	}
	VRHandLeftRoot->SetupAttachment(RootComponent);

	VRHandRightRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VR Hand Right Root"));
	if (!VRHandRightRoot)
	{
		return false;
	}
	VRHandRightRoot->SetupAttachment(RootComponent);


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
#ifdef IN_EDITOR
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
#endif

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	if (bIsSeatedVR)
	{
		VRCameraRoot->AddLocalOffset(FVector::UpVector * SeatedHeightOffset);
		VRHandLeftRoot->AddLocalOffset(FVector::UpVector * SeatedHeightOffset);
		VRHandRightRoot->AddLocalOffset(FVector::UpVector * SeatedHeightOffset);
	}

	UWorld* World = GetWorld();
	if (World && VRHandClassLeft && VRHandClassRight)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.bAllowDuringConstructionScript = true;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FAttachmentTransformRules AttatchmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

		LeftHand = World->SpawnActor(VRHandClassLeft->GetDefaultObject()->GetClass(), &FTransform::Identity, SpawnParameters);
		if (LeftHand)
		{
			LeftHand->SetOwner(this);
			LeftHand->AttachToComponent(VRHandLeftRoot, AttatchmentRules);
		}

		RightHand = World->SpawnActor(VRHandClassRight->GetDefaultObject()->GetClass(), &FTransform::Identity, SpawnParameters);
		if (RightHand)
		{
			RightHand->SetOwner(this);
			RightHand->AttachToComponent(VRHandRightRoot, AttatchmentRules);
		}
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