#pragma once

#include "CoreMinimal.h"

class FHelpers
{
public:
	// Find Objects in Level
	static TArray<AActor*> FindActorsByTag(FName Tag);

	// Load Assets
	static void LoadAllLevelSequenceAssetsFromPath(TArray<FAssetData>& AssetDataList, const FString& Path);
	
	// Read-Write JSON File
	static TSharedPtr<FJsonObject> ReadJson(const FString &JsonFilePath, bool& bOutSuccess, FString& OutMessage);
	static void WriteJson(const FString &JsonFilePath, TSharedPtr<FJsonObject> JsonObject, bool& bOutSuccess, FString& OutMessage);
	static FString ReadStringFromFile(const FString &FilePath, bool& bOutSuccess, FString& OutMessage);
	static void WriteStringToFile(const FString &FilePath, const FString &String, bool& bOutSuccess, FString& OutMessage);
};