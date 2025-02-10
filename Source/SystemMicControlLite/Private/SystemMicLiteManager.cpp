// Copyright 2025 Andrew Bindraw. All Rights Reserved.

#include "SystemMicLiteManager.h"

FSystemMicLiteManager *FSystemMicLiteManager::Instance = nullptr;

FSystemMicLiteManager::FSystemMicLiteManager() 
#if PLATFORM_WINDOWS
: AudioEndpointVolume(nullptr),
	DefaultDevice(nullptr),
	DeviceEnumerator(nullptr),
	DevicesCollection(nullptr),
	PropertyStore(nullptr),
	PolicyConfigVista(nullptr),
	PolicyConfig(nullptr)
#endif
{
#if PLATFORM_WINDOWS
	FWindowsPlatformMisc::CoInitialize();

	CoCreateInstance(__uuidof(CPolicyConfigVistaClient), nullptr, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&PolicyConfigVista);

	// For Win10
	HRESULT Result = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_INPROC, IID_IPolicyConfig2, (LPVOID *)&PolicyConfig);
	if (Result != S_OK)
	{
		Result = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_INPROC, IID_IPolicyConfig1, (LPVOID *)&PolicyConfig);
	}

	// For Win Vista, 7, 8, 8.1
	if (Result != S_OK)
	{
		Result = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_INPROC, IID_IPolicyConfig0, (LPVOID *)&PolicyConfig);
	}

	CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&DeviceEnumerator);
#endif
}

FSystemMicLiteManager *FSystemMicLiteManager::Get()
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
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return FString(TEXT(""));
	}

	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd371405(v=vs.85).aspx, see Return value!
	Result = DefaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (PVOID *)&AudioEndpointVolume);
	if (Result != S_OK)
	{
		return FString(TEXT(""));
	}

	//WCHAR* swDeviceId = reinterpret_cast<wchar_t *>(TCHAR_TO_UTF8(*FString(TEXT(""))));
	WCHAR* swDeviceId = TCHAR_TO_WCHAR(TEXT(""));
	Result = DefaultDevice->GetId(&swDeviceId);
	if (Result != S_OK)
	{
		return FString(TEXT(""));
	}
	DeviceIdStr = FString(WCHAR_TO_TCHAR(swDeviceId));
#endif

	return DeviceIdStr;
}

FString FSystemMicLiteManager::GetDeviceNameFromId(const FString &DeviceId)
{
	FString DeviceName;

	for (TPair<FString, FString>& Device : GetActiveDevices())
	{
		if (Device.Key == DeviceId)
		{
			DeviceName = Device.Value;
		}
	}

	return DeviceName;
}

TMap<FString, FString> FSystemMicLiteManager::GetActiveDevices()
{
	TMap<FString, FString> ActiveDevices;

#if PLATFORM_WINDOWS
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd371400(v=vs.85).aspx, see Return value!
	HRESULT Result = DeviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &DevicesCollection);
	if (Result != S_OK)
	{
		return TMap<FString, FString>();
	}

	UINT CountActiveDevices = 0;
	DevicesCollection->GetCount(&CountActiveDevices);
	LPWSTR pwszID = nullptr;

	for (UINT i = 0; i < CountActiveDevices; i++)
	{
		DevicesCollection->Item(i, &DefaultDevice);
		DefaultDevice->GetId(&pwszID);
		DefaultDevice->OpenPropertyStore(STGM_READ, &PropertyStore);

		PROPVARIANT nameDevice;
		PropVariantInit(&nameDevice);
		PropertyStore->GetValue(PKEY_Device_FriendlyName, &nameDevice);

		ActiveDevices.Add(FString(WCHAR_TO_TCHAR(pwszID)));
		ActiveDevices[FString(WCHAR_TO_TCHAR(pwszID))] = FString(WCHAR_TO_TCHAR(nameDevice.pwszVal));

		CoTaskMemFree(pwszID);
		pwszID = nullptr;
	}

	if (PropertyStore)
	{
		PropertyStore->Release();
		PropertyStore = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}

	if (DevicesCollection)
	{
		DevicesCollection->Release();
		DevicesCollection = nullptr;
	}
#endif

	return ActiveDevices;
}


void FSystemMicLiteManager::SetVolume(float Value)
{
	float MicVolume = this->GetScalarFromValue(Value);

#if PLATFORM_WINDOWS
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return;
	}

	Result = DefaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (PVOID *)&AudioEndpointVolume);
	if (Result != S_OK)
	{
		return;
	}

	Result = AudioEndpointVolume->SetMasterVolumeLevelScalar(MicVolume, nullptr);
	if (Result != S_OK)
	{
		return;
	}

	if (AudioEndpointVolume)
	{
		AudioEndpointVolume->Release();
		AudioEndpointVolume = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
	}
#endif
}

float FSystemMicLiteManager::GetVolume()
{
	float MicVolume = 0.0f;

#if PLATFORM_WINDOWS
	HRESULT Result = DeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &DefaultDevice);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	Result = DefaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (PVOID *)&AudioEndpointVolume);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	Result = AudioEndpointVolume->GetMasterVolumeLevelScalar(&MicVolume);
	if (Result != S_OK)
	{
		return 0.0f;
	}

	if (AudioEndpointVolume)
	{
		AudioEndpointVolume->Release();
		AudioEndpointVolume = nullptr;
	}

	if (DefaultDevice)
	{
		DefaultDevice->Release();
		DefaultDevice = nullptr;
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