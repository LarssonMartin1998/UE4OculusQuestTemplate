#include "HMDHelper.h"

#include "Public/HeadMountedDisplayFunctionLibrary.h"

bool UHMDHelper::bHMDHelperHasInited = false;
EUsedHMD UHMDHelper::UsedHMD = EUsedHMD::UH_None;

void UHMDHelper::Init()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected())
	{
		FName DeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();

		if (DeviceName == TEXT("SteamVR"))
		{
			UsedHMD = EUsedHMD::UH_SteamVR;
		}
		else if (DeviceName == TEXT("OculusHMD"))
		{
			UsedHMD = EUsedHMD::UH_OculusHMD;
		}
		// Unrecognized HMD, defualt to None.
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Unrecognized HMD detected, setting to None, see HMDHelper::Init"));
			UsedHMD = EUsedHMD::UH_None;
		}
	}
	else
	{
		UsedHMD = EUsedHMD::UH_None;
	}

	bHMDHelperHasInited = true;
}

bool UHMDHelper::IsSteamVR()
{ 
	if (!bHMDHelperHasInited)
	{
		Init();
	}

	return UsedHMD == EUsedHMD::UH_SteamVR;
}

bool UHMDHelper::IsOculusHMD()
{
	if (!bHMDHelperHasInited)
	{
		Init();
	}

	return UsedHMD == EUsedHMD::UH_OculusHMD;
}

bool UHMDHelper::IsHMDConnected()
{
	if (!bHMDHelperHasInited)
	{
		Init();
	}

	return UsedHMD == EUsedHMD::UH_None;
}