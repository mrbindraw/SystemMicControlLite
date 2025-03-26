// Copyright 2025 Andrew Bindraw. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SystemMicLiteManager.h"
#include "SystemMicControlLiteBPLibrary.generated.h"

UCLASS()
class SYSTEMMICCONTROLLITE_API USystemMicControlLiteBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	virtual void PostInitProperties() override;

	UFUNCTION(BlueprintPure, Category = "SystemMicControlLite", DisplayName = "Get System Mic Default Device Name")
	static FString GetMicDefaultDeviceName();

	UFUNCTION(BlueprintCallable, Category = "SystemMicControlLite", DisplayName = "Get System Mic Device Id From Name")
	static void GetMicDeviceIdFromName(const FString& InDeviceName, FString& OutDeviceId);

	UFUNCTION(BlueprintCallable, Category = "SystemMicControlLite", DisplayName = "Get System Mic Device Name From Id")
	static void GetMicDeviceNameFromId(const FString& InDeviceId, FString& OutDeviceName);

	UFUNCTION(BlueprintCallable, Category = "SystemMicControlLite", DisplayName = "Set System Mic Volume")
	static void SetMicVolume(float Value);

	/** If DeviceId is empty then it will set Volume value to the default device microphone in the system */
	UFUNCTION(BlueprintCallable, Category = "SystemMicControlLite", DisplayName = "Set System Mic Volume For DeviceId")
	static void SetMicVolumeForDeviceId(const FString& DeviceId, float Volume);

	UFUNCTION(BlueprintPure, Category = "SystemMicControlLite", DisplayName = "Get System Mic Volume")
	static float GetMicVolume();

	/** If DeviceId is empty then it will return volume value from the default device microphone in the system */
	UFUNCTION(BlueprintPure, Category = "SystemMicControlLite", DisplayName = "Get System Mic Volume For DeviceId")
	static float GetMicVolumeForDeviceId(const FString& DeviceId);
	
	UFUNCTION(BlueprintPure, Category = "SystemMicControlLite", DisplayName = "Get System Mic Active Devices")
	static TMap<FString, FString> GetMicActiveDevices();
};
