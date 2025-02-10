// Copyright 2025 Andrew Bindraw. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <mmeapi.h>
#include <ksmedia.h>
#include "PolicyConfig.h"
#include "Windows/HideWindowsPlatformTypes.h"
#endif

class FSystemMicLiteManager
{
	private:
		FSystemMicLiteManager();
		~FSystemMicLiteManager();
		
		static FSystemMicLiteManager *Instance;

#if PLATFORM_WINDOWS
		IAudioEndpointVolume    *AudioEndpointVolume;
		IMMDevice               *DefaultDevice;
		IMMDeviceEnumerator     *DeviceEnumerator;
		IMMDeviceCollection     *DevicesCollection;
		IPropertyStore          *PropertyStore;

		IPolicyConfigVista      *PolicyConfigVista;
		IPolicyConfig           *PolicyConfig;
#endif

	public:
		static FSystemMicLiteManager *Get();
		
		static void DestroyInstance();
		
		FString GetDefaultDeviceName();

		FString GetDefaultDeviceId();

		FString GetDeviceNameFromId(const FString &DeviceId);

		TMap<FString, FString> GetActiveDevices();

		void SetVolume(float Value);

		float GetVolume();

	private:
		FORCEINLINE float GetScalarFromValue(int32 Value);

		FORCEINLINE float GetValueFromScalar(float Value);
};


