#include "VRPawn.h"

#include "Public/HeadMountedDisplayFunctionLibrary.h"
#include "Engine/World.h"
#include "Math/TransformNonVectorized.h"
#include "Math/Vector.h"
#include "Engine/EngineTypes.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"

AVRPawn::AVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	bHasSetupInputComponent = false;

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

	SetupPlayerInputComponent(InputComponent);
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
	UWorld* World = GetWorld();

	// Remove this outer if statement if you have a PCVR HMD to try PIE with.
	if (!World->IsPlayInEditor())
	{
		UE_LOG(LogTemp, Error, TEXT("Not In Editor"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("Not In Editor"));
		}

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
	}

	// This can easily be expanded upon later if you want to support multiple platforms.
	const FName& HMDDeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();
	if (HMDDeviceName != TEXT("OculusHMD"))
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
		VRHandLeftRoot->AddLocalOffset(FVector::UpVector * SeatedHeightOffset);
		VRHandRightRoot->AddLocalOffset(FVector::UpVector * SeatedHeightOffset);
	}

	if (World && VRHandClassLeft && VRHandClassRight)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.bAllowDuringConstructionScript = true;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FAttachmentTransformRules AttatchmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

		LeftHand = Cast<AVRHand>(World->SpawnActor(VRHandClassLeft->GetDefaultObject()->GetClass(), &FTransform::Identity, SpawnParameters));
		if (LeftHand)
		{
			LeftHand->SetOwner(this);
			LeftHand->AttachToComponent(VRHandLeftRoot, AttatchmentRules);
		}

		RightHand = Cast<AVRHand>(World->SpawnActor(VRHandClassRight->GetDefaultObject()->GetClass(), &FTransform::Identity, SpawnParameters));
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
	// Since LeftHand and RightHand is created on BeginPlay, we will manually call this function after. Don't want to be dependant on Unreals internal execution order.
	if (bHasSetupInputComponent || !LeftHand || !RightHand)
	{
		return;
	}

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Teleport_L", IE_Pressed, LeftHand, &AVRHand::OnTeleportLPressed);
	PlayerInputComponent->BindAction("Teleport_R", IE_Pressed, RightHand, &AVRHand::OnTeleportRPressed);
	PlayerInputComponent->BindAction("Teleport_L", IE_Released, LeftHand, &AVRHand::OnTeleportLReleased);
	PlayerInputComponent->BindAction("Teleport_R", IE_Released, RightHand, &AVRHand::OnTeleportRReleased);

	PlayerInputComponent->BindAxis("Thumb_L", LeftHand, &AVRHand::ThumbLAxis);
	PlayerInputComponent->BindAxis("Thumb_R", RightHand, &AVRHand::ThumbRAxis);

	PlayerInputComponent->BindAxis("Pointing_L", LeftHand, &AVRHand::PointingLAxis);
	PlayerInputComponent->BindAxis("Pointing_R", RightHand, &AVRHand::PointingRAxis);

	PlayerInputComponent->BindAxis("Grip_L", LeftHand, &AVRHand::GripLAxis);
	PlayerInputComponent->BindAxis("Grip_R", RightHand, &AVRHand::GripRAxis);

	bHasSetupInputComponent = true;
}