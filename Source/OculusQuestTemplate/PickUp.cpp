#include "PickUp.h"

#include "VRHand.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

APickUp::APickUp()
{
	PrimaryActorTick.bCanEverTick = false;

	GripType = EGripType::GT_Default;

	GripFlex = 0.5f;

	HoverReferenceCount = 0;

	bIsCurrentlyPickedUp = false;

	FresnelIntensityParamName = TEXT("FresnelIntensity");

	AnchorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Anchor Root"));
	if (AnchorRoot)
	{
		SetRootComponent(AnchorRoot);
	}

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	if (StaticMesh)
	{
		StaticMesh->SetupAttachment(RootComponent);
		StaticMesh->SetSimulatePhysics(true);
		StaticMesh->SetCollisionResponseToAllChannels(ECR_Block);
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void APickUp::BeginPlay()
{
	Super::BeginPlay();

	// Init the Dynamic Material Instance
	if (StaticMesh)
	{
		StaticMeshMaterialInstance = StaticMesh->CreateAndSetMaterialInstanceDynamic(0);
		const FMaterialParameterInfo ParamInfo(FresnelIntensityParamName);
		StaticMeshMaterialInstance->GetScalarParameterValue(ParamInfo, FresnelIntensityDefaultValue);
		StaticMeshMaterialInstance->InitializeScalarParameterAndGetIndex(FresnelIntensityParamName, 0.0f, FresnelIntensityParamIndex);
	}
}

void APickUp::OnPickedUp(AVRHand* VRHand)
{
	StaticMesh->SetSimulatePhysics(false);
	bIsCurrentlyPickedUp = true;
}

void APickUp::OnReleased(AVRHand* VRHand)
{
	StaticMesh->SetSimulatePhysics(true);
	bIsCurrentlyPickedUp = false;
}

void APickUp::OnHoverSelected(AVRHand* VRHand)
{
	if (bIsCurrentlyPickedUp)
	{
		return;
	}

	if (++HoverReferenceCount < 2)
	{
		StaticMeshMaterialInstance->SetScalarParameterByIndex(FresnelIntensityParamIndex, FresnelIntensityDefaultValue);
	}
}

void APickUp::OnHoverDeselected(AVRHand* VRHand)
{
	if (--HoverReferenceCount < 1)
	{
		HoverReferenceCount = 0;
		StaticMeshMaterialInstance->SetScalarParameterByIndex(FresnelIntensityParamIndex, 0.0f);
	}

}