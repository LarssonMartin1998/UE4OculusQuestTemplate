#include "VRHand.h"

#include "HMDHelper.h"
#include "Components/SceneComponent.h"
#include "Public/MotionControllerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"

AVRHand::AVRHand()
{
	PrimaryActorTick.bCanEverTick = true;

	CreateVRComponents();
}

bool AVRHand::CreateVRComponents()
{
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	if (!MotionController)
	{
		return false;
	}
	SetRootComponent(MotionController);

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hand Mesh"));
	if (!HandMesh)
	{
		return false;
	}
	HandMesh->SetupAttachment(RootComponent);

	BoxColliderAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("Box Collider Anchor"));
	if (!BoxColliderAnchor)
	{
		return false;
	}
	BoxColliderAnchor->SetupAttachment(RootComponent);

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	if (!BoxCollider)
	{
		return false;
	}
	BoxCollider->SetupAttachment(BoxColliderAnchor);
	BoxCollider->SetBoxExtent(FVector(100.0f, 25.0f, 25.0f));
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxCollider->SetCollisionObjectType(ECC_Pawn);
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollider->SetSimulatePhysics(false);
	BoxCollider->SetRelativeLocation(FVector(90.0f, 0.0f, 0.0f));
	BoxCollider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	return true;
}

void AVRHand::BeginPlay()
{
	Super::BeginPlay();

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AVRHand::OnOverlapBegin);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &AVRHand::OnOverlapEnd);

	if (UHMDHelper::IsSteamVR())
	{
		HandMesh->AddLocalOffset(SteamVRLocationOffset);
		HandMesh->AddLocalRotation(SteamVRRotationOffset);

		FRotator ColliderRotOffset(SteamVRRotationOffset.Yaw, SteamVRRotationOffset.Pitch, SteamVRRotationOffset.Roll);
	}

	BoxColliderAnchor->SetRelativeRotation(HandMesh->GetComponentRotation());
}

void AVRHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Pick something up.
	if (!CurrentPickUp && ClosestPickUp && GripAxis >= 0.5f)
	{
		CurrentPickUp = ClosestPickUp;
		CurrentPickUp->OnPickedUp(this);

		// This func sets closestpickup to nullptr
		InternalHoverDeselectClosestPickUp();

		FHitResult OutSweepHitResult;
		CurrentPickUp->SetActorLocation(RootComponent->GetComponentLocation(), false, &OutSweepHitResult, ETeleportType::ResetPhysics);
		// TODO: Attatch to socket.
	}

	// Drop a pick up.
	if (CurrentPickUp && GripAxis < 0.5f)
	{
		CurrentPickUp->OnReleased(this);

		CurrentPickUp = nullptr;
		ClosestPickUp = nullptr;
	}

	const int NumElements = OverlappedPickUps.Num();
	if (NumElements == 1 && !ClosestPickUp)
	{
		InternalHoverSelectNewPickUp(OverlappedPickUps[0]);
		return;
	}
	else if (NumElements > 1)
	{
		const FVector& RootLocation = GetActorLocation();
		const FVector& RootForward = GetActorForwardVector();

		// Just make sure we always have a ClosestPickUp.
		ClosestPickUp = ClosestPickUp ? ClosestPickUp : OverlappedPickUps[0];

		FVector HandToClosestPickUp = (ClosestPickUp->GetActorLocation() - RootLocation);
		HandToClosestPickUp.Normalize();
		float HandDotClosestPickUp = FVector::DotProduct(RootForward, HandToClosestPickUp);

		for (APickUp* PickUp : OverlappedPickUps)
		{
			if (PickUp == ClosestPickUp)
			{
				continue;
			}

			FVector HandToNewPickUp = (PickUp->GetActorLocation() - RootLocation);
			HandToNewPickUp.Normalize();
			const float HandDotNewPickUp = FVector::DotProduct(RootForward, HandToNewPickUp);

			if (HandDotNewPickUp > HandDotClosestPickUp)
			{
				InternalHoverDeselectClosestPickUp();
				InternalHoverSelectNewPickUp(PickUp);
				ClosestPickUp = PickUp;

				HandDotClosestPickUp = HandDotNewPickUp;

				// Let's just be happy with this result??
				if (HandDotClosestPickUp >= 0.975f)
				{
					break;
				}
			}
		}
	}
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

	if (UHMDHelper::IsSteamVR())
	{
		bIsPointing = !bIsPointing;
	}
}

void AVRHand::PointingRAxis(float Axis)
{
	bIsPointing = Axis > 0.5f;

	if (UHMDHelper::IsSteamVR())
	{
		bIsPointing = !bIsPointing;
	}
}

void AVRHand::ThumbLAxis(float Axis)
{
	bIsThumbUp = Axis > 0.5f;

	if (UHMDHelper::IsSteamVR())
	{
		bIsThumbUp = !bIsThumbUp;
	}
}

void AVRHand::ThumbRAxis(float Axis)
{
	bIsThumbUp = Axis > 0.5f;

	if (UHMDHelper::IsSteamVR())
	{
		bIsThumbUp = !bIsThumbUp;
	}
}

void AVRHand::GripLAxis(float Axis)
{
	GripAxis = Axis;
}

void AVRHand::GripRAxis(float Axis)
{
	GripAxis = Axis;
}

void AVRHand::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CurrentPickUp)
	{
		return;
	}

	APickUp* PickUp = Cast<APickUp>(OtherActor);
	if (PickUp)
	{
		OverlappedPickUps.Add(PickUp);
	}
}

void AVRHand::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APickUp* PickUp = Cast<APickUp>(OtherActor);
	if (PickUp)
	{
		OverlappedPickUps.Remove(PickUp);

		if (PickUp == ClosestPickUp)
		{
			InternalHoverDeselectClosestPickUp();
		}
	}
}

void AVRHand::InternalHoverSelectNewPickUp(APickUp* PickUp)
{
	if (PickUp)
	{
		PickUp->OnHoverSelected(this);
		ClosestPickUp = PickUp;
	}
}

void AVRHand::InternalHoverDeselectClosestPickUp()
{
	if (ClosestPickUp)
	{
		ClosestPickUp->OnHoverDeselected(this);
		ClosestPickUp = nullptr;
	}
}