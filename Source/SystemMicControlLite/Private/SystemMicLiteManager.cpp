// Copyright 2025 Andrew Bindraw. All Rights Reserved.

#include "SystemMicLiteManager.h"

DEFINE_LOG_CATEGORY(LogSystemMicLiteManager);

FSystemMicLiteManager* FSystemMicLiteManager::Instance = nullptr;

FSystemMicLiteManager::FSystemMicLiteManager() 
#if PLATFORM_WINDOWS
: PolicyConfigVista(nullptr),
	PolicyConfig(nullptr),
	DeviceEnumerator(nullptr)
#endif
{

}

void FSystemMicLiteManager::Init()
{
#if PLATFORM_WINDOWS
	FWindowsPlatformMisc::CoInitialize();

	CoCreateInstance(__uuidof(CPolicyConfigVistaClient), nullptr, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&PolicyConfigVista);

	// For Win10
	HRESULT Result = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_INPROC, IID_IPolicyConfig2, (LPVOID*)&PolicyConfig);
	if (Result != S_OK)
	{
		Result = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_INPROC, IID_IPolicyConfig1, (LPVOID*)&PolicyConfig);
	}

	// For Win Vista, 7, 8, 8.1
	if (Result != S_OK)
	{
		Result = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_INPROC, IID_IPolicyConfig0, (LPVOID*)&PolicyConfig);
	}

	CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&DeviceEnumerator);
#endif
}

FSystemMicLiteManager* FSystemMicLiteManager::Get()
{
	if(Instance == nullptr)
	{
		Instance = new FSystemMicLiteManager;
	}
	
	return Instance;
}

FSystemMicLiteManager::~FSystemMicLiteManager()
{
#if PLATFORM_WINDOWS
	FWindowsPlatformMisc::CoUninitialize();
#endif
}

void FSystemMicLiteManager::DestroyInstance()
{
	if(Instance != nullptr)
	{
		delete Instance;
		Instance = nullptr;
	}
}

FString FSystemMicLiteManager::GetDefaultDeviceName()
{
	return GetDeviceNameFromId(GetDefaultDeviceId());
}

FString FSystemMicLiteManager::GetDefaultDeviceId()
{
	FString DeviceIdStr;

#if PLATFORM_WINDOWS
	TComPtr<IMMDevice> Device = GetDevice();
	if (!Device.IsValid())
	{
		return FString(TEXT(""));
	}

	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd371405(v=vs.85).aspx, see Return value!
	TComPtr<IAudioEndpointVolume> AudioEndpointVolume = GetAudioEndpointVolume(Device);
	if (!AudioEndpointVolume.IsValid())
	{
		return FString(TEXT(""));
	}

	WCHAR* swDeviceId = TCHAR_TO_WCHAR(TEXT(""));
	HRESULT Result = Device->GetId(&swDeviceId);
	if (Result != S_OK)
	{
		UE_LOG(LogSystemMicLiteManager, Warning, TEXT("Result != S_OK, Device->GetId, [%s], line: %d"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__);
		return FString(TEXT(""));
	}
	DeviceIdStr = FString(WCHAR_TO_TCHAR(swDeviceId));

	CoTaskMemFree(swDeviceId);
	swDeviceId = nullptr;
#endif

	return DeviceIdStr;
}

FString FSystemMicLiteManager::GetDeviceIdFromName(const FString& DeviceName)
{
	FString DeviceId;

	for (const TPair<FString, FString>& Device : GetActiveDevices())
	{
		if (Device.Value == DeviceName)
		{
			DeviceId = Device.Key;
			break;
		}
	}

	return DeviceId;
}

FString FSystemMicLiteManager::GetDeviceNameFromId(const FString& DeviceId)
{
	FString DeviceName;

	for (const TPair<FString, FString>& Device : GetActiveDevices())
	{
		if (Device.Key == DeviceId)
		{
			DeviceName = Device.Value;
			break;
		}
	}

	return DeviceName;
}

TMap<FString, FString> FSystemMicLiteManager::GetActiveDevices()
{
	TMap<FString, FString> ActiveDevices;

#if PLATFORM_WINDOWS
	TComPtr<IMMDeviceCollection> DevicesCollection;
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd371400(v=vs.85).aspx, see Return value!
	HRESULT Result = DeviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &DevicesCollection);
	if (Result != S_OK)
	{
		UE_LOG(LogSystemMicLiteManager, Warning, TEXT("Result != S_OK, DeviceEnumerator->EnumAudioEndpoints, [%s], line: %d"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__);
		return TMap<FString, FString>();
	}

	UINT CountActiveDevices = 0;
	Result = DevicesCollection->GetCount(&CountActiveDevices);
	if (Result != S_OK)
	{
		UE_LOG(LogSystemMicLiteManager, Warning, TEXT("Result != S_OK, DevicesCollection->GetCount, [%s], line: %d"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__);
		return TMap<FString, FString>();
	}

	TComPtr<IMMDevice> Device;
	TComPtr<IPropertyStore> PropertyStore;
	LPWSTR pwszID = nullptr;

	for (UINT i = 0; i < CountActiveDevices; i++)
	{
		DevicesCollection->Item(i, &Device);
		Device->GetId(&pwszID);
		Device->OpenPropertyStore(STGM_READ, &PropertyStore);

		PROPVARIANT nameDevice;
		PropVariantInit(&nameDevice);
		PropertyStore->GetValue(PKEY_Device_FriendlyName, &nameDevice);

		ActiveDevices.Add(FString(WCHAR_TO_TCHAR(pwszID)));
		ActiveDevices[FString(WCHAR_TO_TCHAR(pwszID))] = FString(WCHAR_TO_TCHAR(nameDevice.pwszVal));

		PropVariantClear(&nameDevice);

		CoTaskMemFree(pwszID);
		pwszID = nullptr;
	}
#endif

	return ActiveDevices;
}


void FSystemMicLiteManager::SetVolume(float Value, const FString& DeviceId)
{
	if (FMath::IsNearlyEqual(Value, GetVolume(DeviceId), 0.001f))
	{
		return;
	}

	float MicVolume = GetScalarFromValue(Value);

#if PLATFORM_WINDOWS
	TComPtr<IAudioEndpointVolume> AudioEndpointVolume = GetAudioEndpointVolume(DeviceId);
	if (!AudioEndpointVolume.IsValid())
	{
		return;
	}

	HRESULT Result = AudioEndpointVolume->SetMasterVolumeLevelScalar(MicVolume, nullptr);
	if (Result != S_OK)
	{
		UE_LOG(LogSystemMicLiteManager, Warning, TEXT("Result != S_OK, AudioEndpointVolume->SetMasterVolumeLevelScalar, [%s], line: %d"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__);
		return;
	}
#endif
}

float FSystemMicLiteManager::GetVolume(const FString& DeviceId)
{
	float MicVolume = 0.0f;

#if PLATFORM_WINDOWS
	TComPtr<IAudioEndpointVolume> AudioEndpointVolume = GetAudioEndpointVolume(DeviceId);
	if (!AudioEndpointVolume.IsValid())
	{
		return 0.0f;
	}

	HRESULT Result = AudioEndpointVolume->GetMasterVolumeLevelScalar(&MicVolume);
	if (Result != S_OK)
	{
		UE_LOG(LogSystemMicLiteManager, Warning, TEXT("Result != S_OK, AudioEndpointVolume->GetMasterVolumeLevelScalar, [%s], line: %d"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__);
		return 0.0f;
	}
#endif

	return GetValueFromScalar(MicVolume);
}

float FSystemMicLiteManager::GetScalarFromValue(int32 Value)
{
	return FMath::Abs(Value) >= 100.0f ? 1.0f : Value / 100.0f;
}

float FSystemMicLiteManager::GetValueFromScalar(float Value)
{
	return FMath::RoundToFloat(FMath::Abs(Value) > 0.0f ? Value * 100.0f : 0.0f);
}

TComPtr<IMMDevice> FSystemMicLiteManager::GetDevice(const FString& DeviceId)
{
	TComPtr<IMMDevice> Device;
	HRESULT Result = S_OK;

	if (DeviceId.IsEmpty())
	{
		Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &Device);
		if (Result != S_OK)
		{
			UE_LOG(LogSystemMicLiteManager, Warning, TEXT("Result != S_OK, DeviceEnumerator->GetDefaultAudioEndpoint, [%s], line: %d"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__);
			return nullptr;
		}
	}
	else
	{
		Result = DeviceEnumerator->GetDevice(TCHAR_TO_WCHAR(*DeviceId), &Device);
		if (Result != S_OK)
		{
			UE_LOG(LogSystemMicLiteManager, Warning, TEXT("Result != S_OK, DeviceEnumerator->GetDevice, [%s], line: %d"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__);
			return nullptr;
		}
	}

	return Device;
}

TComPtr<IAudioEndpointVolume> FSystemMicLiteManager::GetAudioEndpointVolume(const TComPtr<IMMDevice>& Device)
{
	if (!Device.IsValid())
	{
		return nullptr;
	}

	TComPtr<IAudioEndpointVolume> AudioEndpointVolume;
	HRESULT Result = Device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (PVOID*)&AudioEndpointVolume);
	if (Result != S_OK)
	{
		UE_LOG(LogSystemMicLiteManager, Warning, TEXT("Result != S_OK, Device->Activate, [%s], line: %d"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__);
		return nullptr;
	}

	return AudioEndpointVolume;
}

TComPtr<IAudioEndpointVolume> FSystemMicLiteManager::GetAudioEndpointVolume(const FString& DeviceId)
{
	TComPtr<IMMDevice> Device = GetDevice(DeviceId);
	if (!Device.IsValid())
	{
		return nullptr;
	}

	TComPtr<IAudioEndpointVolume> AudioEndpointVolume = GetAudioEndpointVolume(Device);
	if (!AudioEndpointVolume.IsValid())
	{
		return nullptr;
	}

	return AudioEndpointVolume;
}
