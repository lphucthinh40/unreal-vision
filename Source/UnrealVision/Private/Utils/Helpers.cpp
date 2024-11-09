#include "Utils/Helpers.h"
#include <Kismet/GameplayStatics.h>

#include "LevelSequence.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"

TArray<AActor*> FHelpers::FindActorsByTag(FName Tag) {
	TArray<AActor*> FoundActors;
	FWorldContext* world = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World = world->World();
	if (World) {
		UGameplayStatics::GetAllActorsWithTag(World, Tag, FoundActors);
		if (FoundActors.Num() > 0) {
			return FoundActors;
		}
	}
	return FoundActors;
}

void FHelpers::LoadAllLevelSequenceAssetsFromPath(TArray<FAssetData>& AssetDataList, const FString& Path) {
	// Load Asset Registry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// Create Asset Filter
	FARFilter Filter;
	Filter.PackagePaths.Add(*Path);
	Filter.ClassPaths.Add(ULevelSequence::StaticClass()->GetClassPathName());
	Filter.bRecursivePaths = true;

	// Get Assets
	AssetRegistry.GetAssets(Filter, AssetDataList);
	UE_LOG(LogTemp, Log, TEXT("Loaded %d Assets"), AssetDataList.Num());
}

TSharedPtr<FJsonObject> FHelpers::ReadJson(const FString &JsonFilePath, bool& bOutSuccess, FString& OutMessage) {
	FString JsonString = ReadStringFromFile(JsonFilePath, bOutSuccess, OutMessage);
	if (!bOutSuccess) {
		return nullptr;
	}
	TSharedPtr<FJsonObject> RetJsonObject;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonString), RetJsonObject)) {
		bOutSuccess = false;
		OutMessage = FString::Printf(TEXT("Read Json Failed - Was not able to deserialize the json string. Is it the right format? - '%s'"), *JsonString);
		return nullptr;
	}
	bOutSuccess = true;
	OutMessage = FString::Printf(TEXT("Read Json Succeeded - '%s'"), *JsonFilePath);
	return RetJsonObject;
}

void FHelpers::WriteJson(const FString &JsonFilePath, TSharedPtr<FJsonObject> JsonObject, bool& bOutSuccess, FString& OutMessage) {
	FString JsonString;
	if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&JsonString, 0))) {
		bOutSuccess = false;
		OutMessage = FString::Printf(TEXT("Write Json Failed - Was not able to serialize the json to string. Is the JsonObject valid?"));
		return;
	}
	WriteStringToFile(JsonFilePath, JsonString, bOutSuccess, OutMessage);
	if (!bOutSuccess) {
		return;
	}
	bOutSuccess = true;
	OutMessage = FString::Printf(TEXT("Write Json Succeeded - '%s'"), *JsonFilePath);
}

FString FHelpers::ReadStringFromFile(const FString &FilePath, bool& bOutSuccess, FString& OutMessage) {
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		bOutSuccess = false;
		OutMessage = FString::Printf(TEXT("Read String From File Failed - File doesn't exist - '%s'"), *FilePath);
		return "";
	}
	FString RetString = "";
	if (!FFileHelper::LoadFileToString(RetString, *FilePath))
	{
		bOutSuccess = false;
		OutMessage = FString::Printf(TEXT("Read String From File Failed - Was not able to read file. Is this a text file? - '%s'"), *FilePath);
		return "";
	}
	bOutSuccess = true;
	OutMessage = FString::Printf(TEXT("Read String From File Succeeded - '%s'"), *FilePath);
	return RetString;
}

void FHelpers::WriteStringToFile(const FString &FilePath, const FString &String, bool& bOutSuccess, FString& OutMessage) {
	if (!FFileHelper::SaveStringToFile(String, *FilePath))
	{
		bOutSuccess = false;
		OutMessage = FString::Printf(TEXT("Write String To File Failed - Was not able to write to file. Is your file read only? Is the path valid? - '%s'"), *FilePath);
		return;
	}
	bOutSuccess = true;
	OutMessage = FString::Printf(TEXT("Write String To File Succeeded - '%s'"), *FilePath);
}
