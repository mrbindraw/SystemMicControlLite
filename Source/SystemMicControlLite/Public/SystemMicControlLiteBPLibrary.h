// Copyright 2025 Andrew Bindraw. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SystemMicLiteManager.h"
#include "SystemMicControlLiteBPLibrary.generated.h"

UCLASS()
class SYSTEMMICCONTROLLITE_API USystemMicControlLiteBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, Category = "SystemMicControlLite", DisplayName = "Get System Mic Default Device Name")
	static FString GetMicDefaultDeviceName();

	UFUNCTION(BlueprintCallable, Category = "SystemMicControlLite", DisplayName = "Set System Mic Volume")
	static void SetMicVolume(float Value);

	UFUNCTION(BlueprintPure, Category = "SystemMicControlLite", DisplayName = "Get System Mic Volume")
	static float GetMicVolume();
	
	UFUNCTION(BlueprintPure, Category = "SystemMicControlLite", DisplayName = "Get System Mic Active Devices")
	static TMap<FString, FString> GetMicActiveDevices();

};
