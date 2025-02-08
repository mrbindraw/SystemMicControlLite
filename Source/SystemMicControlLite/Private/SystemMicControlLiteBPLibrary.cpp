// Copyright 2025 Andrew Bindraw. All Rights Reserved.

#include "SystemMicControlLiteBPLibrary.h"
#include "SystemMicControlLite.h"

USystemMicControlLiteBPLibrary::USystemMicControlLiteBPLibrary(const FObjectInitializer &ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

FString USystemMicControlLiteBPLibrary::GetMicDefaultDeviceName()
{
	return FSystemMicLiteManager::Get()->GetDefaultDeviceName();
}

void USystemMicControlLiteBPLibrary::SetMicVolume(float Value)
{
	FSystemMicLiteManager::Get()->SetVolume(Value);
}

float USystemMicControlLiteBPLibrary::GetMicVolume()
{
	return FSystemMicLiteManager::Get()->GetVolume();
}

TMap<FString, FString> USystemMicControlLiteBPLibrary::GetMicActiveDevices()
{
	return FSystemMicLiteManager::Get()->GetActiveDevices();
}