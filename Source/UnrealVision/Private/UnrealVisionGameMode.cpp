// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealVisionGameMode.h"

#include "UnrealVisionPawn.h"
#include "CaptureManager.h"
#include "AnnotationManager.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Misc/FileHelper.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlayer.h"
#include "Utils/Helpers.h"
#include "Serialization/JsonSerializer.h" // Json
#include "JsonObjectConverter.h" // JsonUtilities
#include "Blueprint/UserWidget.h"

AUnrealVisionGameMode::AUnrealVisionGameMode() {
	DefaultPawnClass = AUnrealVisionPawn::StaticClass();
	ConfigFilePath = FPaths::ProjectContentDir() + "config.json";
	LevelSequenceAssets = {};
	bCameraStreamEnabled = false;
	bGeneratingDataset = false;
	CaptureIndex = 0;
	LoadGameModeConfigurations(ConfigFilePath);
	LoadInGameWidgetClass();	
}

void AUnrealVisionGameMode::BeginPlay() {
	MapName = UWorld::RemovePIEPrefix(GetWorld()->GetMapName());
    Super::BeginPlay();
	LoadAvailableLevelSequences();
	SetActiveLevelSequence(0);
	CaptureManager = GetWorld()->SpawnActor<ACaptureManager>();
	// AnnotationManager = GetWorld()->SpawnActor<AAnnotationManager>();
	CreateInGameWidget();
}

TArray<FString> AUnrealVisionGameMode::GetSelectableLevelSequences() {
	TArray<FString> LevelSequenceNames = {};
	for (const FAssetData Asset : LevelSequenceAssets) {
		if (Asset.IsValid()) {
			LevelSequenceNames.Add(Asset.AssetName.ToString());
		}
	}
	return LevelSequenceNames;
}

void AUnrealVisionGameMode::LoadGameModeConfigurations(const FString &ConfigPath) {
	bool OutSuccess;
	FString OutMessage;
	Config = ReadConfigFromJsonFile(ConfigPath, OutSuccess, OutMessage);
	if (!OutSuccess) {
		UE_LOG(LogTemp, Warning, TEXT("RESULT: %s"), *OutMessage);
	}
}

void AUnrealVisionGameMode::SaveGameModeConfigurations(const FString &ConfigPath) {
	bool OutSuccess;
	FString OutMessage;
	WriteConfigToJsonFile(ConfigPath, OutSuccess, OutMessage);
	if (!OutSuccess) {
		UE_LOG(LogTemp, Warning, TEXT("RESULT: %s"), *OutMessage);
	}
}

void AUnrealVisionGameMode::LoadAvailableLevelSequences() {
	FString SequencePath = FString::Printf(TEXT("/Game/Sequences/%s"), *MapName);
	FHelpers::LoadAllLevelSequenceAssetsFromPath(LevelSequenceAssets, SequencePath);
	UE_LOG(LogTemp, Error, TEXT("LOADED %d SEQUENCES FOR THIS MAP (%s)"), LevelSequenceAssets.Num(), *MapName);
}

void AUnrealVisionGameMode::SetActiveLevelSequence(int index=0) {
	if (index>=0 && index<LevelSequenceAssets.Num()) {
		ULevelSequence* NewLevelSequenceAsset = Cast<ULevelSequence>(LevelSequenceAssets[index].GetAsset());
		if (!IsValid(LevelSequenceActor)) {
			LevelSequenceActor = GetWorld()->SpawnActor<ALevelSequenceActor>();
		} else {
			LevelSequenceActor->GetSequencePlayer()->Stop();
		}
		LevelSequenceActor->SetSequence(NewLevelSequenceAsset);
		FMovieSceneSequencePlaybackParams startParams = {};
		startParams.PositionType = EMovieScenePositionType::Time;
		startParams.Time = 0;
		startParams.UpdateMethod = EUpdatePositionMethod::Jump;
		LevelSequenceActor->GetSequencePlayer()->SetPlaybackPosition(startParams);
		LevelSequenceActor->GetSequencePlayer()->Play();
		ActiveLevelSequenceIndex = index;
		// Create new annotation manager for this level sequence
		if (IsValid(AnnotationManager)) {
			AnnotationManager->Destroy();
		}
		AnnotationManager = GetWorld()->SpawnActor<AAnnotationManager>();
	}
}

void AUnrealVisionGameMode::ResetLevelSequencePlayer() {
	if (IsValid(LevelSequenceActor)) {
		FMovieSceneSequencePlaybackParams startParams = {};
		startParams.PositionType = EMovieScenePositionType::Time;
		startParams.Time = 0;
		startParams.UpdateMethod = EUpdatePositionMethod::Jump;
		LevelSequenceActor->GetSequencePlayer()->SetPlaybackPosition(startParams);
	}
}

void AUnrealVisionGameMode::JumpToLevelSequenceFrame(int32 Frame) {
	if (IsValid(LevelSequenceActor)) {
		FMovieSceneSequencePlaybackParams startParams = {};
		startParams.PositionType = EMovieScenePositionType::Frame;
		startParams.Frame = FFrameTime(Frame);
		startParams.UpdateMethod = EUpdatePositionMethod::Jump;
		LevelSequenceActor->GetSequencePlayer()->SetPlaybackPosition(startParams);
	}
}

bool AUnrealVisionGameMode::GetLevelSequencePlayerPlaybackState() {
	return LevelSequenceActor->GetSequencePlayer()->IsPlaying();
}


void AUnrealVisionGameMode::ToggleLevelSequencePlayback() {
	if (LevelSequenceActor->GetSequencePlayer()->IsPlaying()) {
		LevelSequenceActor->GetSequencePlayer()->Pause();
	} else {
		LevelSequenceActor->GetSequencePlayer()->Play();
	}
}

void AUnrealVisionGameMode::LoadInGameWidgetClass() {
	static ConstructorHelpers::FClassFinder<UUserWidget> UserWidgetClass (TEXT ("/Game/Widgets/BP_InGameWidget.BP_InGameWidget_C"));
	InGameWidgetClass = UserWidgetClass.Class;
}

void AUnrealVisionGameMode::CreateInGameWidget() {
	InGameWidget = CreateWidget<UUserWidget> (GetWorld(), InGameWidgetClass);
	InGameWidget->AddToViewport();
}

FString AUnrealVisionGameMode::GetMapName() {
	return MapName;
}

FUnrealVisionConfig AUnrealVisionGameMode::ReadConfigFromJsonFile(const FString &JsonFilePath, bool& bOutSuccess, FString& OutMessage) {
    TSharedPtr<FJsonObject> JsonObject = FHelpers::ReadJson(JsonFilePath, bOutSuccess, OutMessage);
    if (!bOutSuccess) {
        return FUnrealVisionConfig();
    }
    FUnrealVisionConfig RetStruct;
    if (!FJsonObjectConverter::JsonObjectToUStruct<FUnrealVisionConfig>(JsonObject.ToSharedRef(), &RetStruct)) {
        bOutSuccess = false;
        OutMessage = FString::Printf(TEXT("Read Struct Json Failed - Was not able to convert the json object to your desired structure. Is it the right format / struct? - '%s'"), *JsonFilePath);
        return FUnrealVisionConfig();
    }
    bOutSuccess = true;
    OutMessage = FString::Printf(TEXT("Read Struct Json Succeeded - '%s'"), *JsonFilePath);
    return RetStruct;
}

void AUnrealVisionGameMode::WriteConfigToJsonFile(const FString &JsonFilePath, bool& bOutSuccess, FString& OutMessage)
{
	// Try to convert struct to generic json object
	TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(Config);
	if (JsonObject == nullptr)
	{
		bOutSuccess = false;
		OutMessage = FString::Printf(TEXT("Write Struct Json Failed - Was not able to convert the struct to a json object. This shouldn't really happen."));
		return;
	}

	// Try to write json to file
	FHelpers::WriteJson(JsonFilePath, JsonObject, bOutSuccess, OutMessage);
}

void AUnrealVisionGameMode::GenerateDataset() {
	FString LevelSequenceName = LevelSequenceAssets[ActiveLevelSequenceIndex].AssetName.ToString();
	CurrentSavePath = FString::Printf(TEXT("%sDatasets/%s"), *FPaths::ProjectSavedDir(), *LevelSequenceName);
	ResetLevelSequencePlayer();
	bGeneratingDataset = true;
	GeneratingDatasetProgress = 0.0;
	CaptureManager->SaveAllCameraMetadataToFile(CurrentSavePath, "Cameras");
}

void AUnrealVisionGameMode::CancelGenerateDataset() {
	bGeneratingDataset = false;
	ResetLevelSequencePlayer();
	GeneratingDatasetProgress = 0.0;
	CaptureIndex = 0;
}

bool AUnrealVisionGameMode::GetGeneratingDatasetStatus() {
	return bGeneratingDataset;
}

double AUnrealVisionGameMode::GetGeneratingDatasetProgress() {
	return GeneratingDatasetProgress;
}