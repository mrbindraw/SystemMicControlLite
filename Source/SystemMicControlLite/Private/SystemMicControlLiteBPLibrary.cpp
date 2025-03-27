// Copyright 2025 Andrew Bindraw. All Rights Reserved.

#include "SystemMicControlLiteBPLibrary.h"
#include "SystemMicControlLite.h"

USystemMicControlLiteBPLibrary::USystemMicControlLiteBPLibrary(const FObjectInitializer &ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

void USystemMicControlLiteBPLibrary::PostInitProperties()
{
	Super::PostInitProperties();

	FSystemMicLiteManager::Get()->Init();
}

FString USystemMicControlLiteBPLibrary::GetMicDefaultDeviceName()
{
	return FSystemMicLiteManager::Get()->GetDefaultDeviceName();
}

void USystemMicControlLiteBPLibrary::GetMicDeviceIdFromName(const FString& InDeviceName, FString& OutDeviceId)
{
	OutDeviceId = FSystemMicLiteManager::Get()->GetDeviceIdFromName(InDeviceName);
}

void USystemMicControlLiteBPLibrary::GetMicDeviceNameFromId(const FString& InDeviceId, FString& OutDeviceName)
{
	OutDeviceName = FSystemMicLiteManager::Get()->GetDeviceNameFromId(InDeviceId);
}

void USystemMicControlLiteBPLibrary::SetMicVolume(float Value)
{
	FSystemMicLiteManager::Get()->SetVolume(Value);
}

void USystemMicControlLiteBPLibrary::SetMicVolumeForDeviceId(const FString& DeviceId, float Volume)
{
	FSystemMicLiteManager::Get()->SetVolume(Volume, DeviceId);
}

float USystemMicControlLiteBPLibrary::GetMicVolume()
{
	return FSystemMicLiteManager::Get()->GetVolume();
}

float USystemMicControlLiteBPLibrary::GetMicVolumeForDeviceId(const FString& DeviceId)
{
	return FSystemMicLiteManager::Get()->GetVolume(DeviceId);
}

TMap<FString, FString> USystemMicControlLiteBPLibrary::GetMicActiveDevices()
{
	return FSystemMicLiteManager::Get()->GetActiveDevices();
}