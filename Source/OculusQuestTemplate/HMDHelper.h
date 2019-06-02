#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HMDHelper.generated.h"

// Just add platforms here for whatever youre targeting.
UENUM(BlueprintType)
enum class EUsedHMD : uint8
{
	UH_SteamVR UMETA(DisplayName = "Steam VR"),
	UH_OculusHMD UMETA(DisplayName = "Oculus HMD"),
	UH_None UMETA(DisplayName = "None")
};

UCLASS()
class OCULUSQUESTTEMPLATE_API UHMDHelper : public UObject
{
	GENERATED_BODY()

public:

	
	static bool IsSteamVR();

	
	static bool IsOculusHMD();

	
	static bool IsHMDConnected();

private:

	static void Init();

	static EUsedHMD UsedHMD;

	static bool bHMDHelperHasInited;

};