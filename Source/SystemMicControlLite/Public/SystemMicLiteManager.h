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
#include "Microsoft/COMPointer.h"
#include "PolicyConfig.h"
#include "Windows/HideWindowsPlatformTypes.h"
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogSystemMicLiteManager, Log, All);

class FSystemMicLiteManager
{
	private:
		FSystemMicLiteManager();
		~FSystemMicLiteManager();
		
		static FSystemMicLiteManager* Instance;

#if PLATFORM_WINDOWS
		TComPtr<IPolicyConfigVista>		PolicyConfigVista;
		TComPtr<IPolicyConfig>			PolicyConfig;
		TComPtr<IMMDeviceEnumerator>	DeviceEnumerator;
#endif

	public:
		static FSystemMicLiteManager* Get();
		
		static void DestroyInstance();

		void Init();
		
		FString GetDefaultDeviceName();

		FString GetDefaultDeviceId();

		FString GetDeviceIdFromName(const FString& DeviceName);

		FString GetDeviceNameFromId(const FString& DeviceId);

		TMap<FString, FString> GetActiveDevices();

		void SetVolume(float Value, const FString& DeviceId = FString(TEXT("")));

		float GetVolume(const FString& DeviceId = FString(TEXT("")));

	private:
		FORCEINLINE float GetScalarFromValue(int32 Value);

		FORCEINLINE float GetValueFromScalar(float Value);

#if PLATFORM_WINDOWS
		TComPtr<IMMDevice> GetDevice(const FString& DeviceId = FString(TEXT("")));
		TComPtr<IAudioEndpointVolume> GetAudioEndpointVolume(const TComPtr<IMMDevice>& Device);
		TComPtr<IAudioEndpointVolume> GetAudioEndpointVolume(const FString& DeviceId = FString(TEXT("")));
#endif
};


