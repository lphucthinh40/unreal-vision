// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Misc/DateTime.h"

#include "UnrealVisionGameMode.generated.h"

USTRUCT()
struct FUnrealVisionConfig{
	
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "UnrealVision Configurations")
	FString OutputPath = "Output";

	UPROPERTY(EditAnywhere, Category = "UnrealVision Configurations")
	FString DefaultSceneName = "DefaultScene";

	UPROPERTY(EditAnywhere, Category = "UnrealVision Configurations")
	FString DefaultLevelName = "BasicMap";

	UPROPERTY(EditAnywhere, Category = "UnrealVision Configurations")
	int ImageWidth = 1080;

	UPROPERTY(EditAnywhere, Category = "UnrealVision Configurations")
	int ImageHeight = 1920;

	UPROPERTY(EditAnywhere, Category = "UnrealVision Configurations")
	int FOV = 90;

	UPROPERTY(EditAnywhere, Category = "UnrealVision Configurations")
	TArray<FTransform> CameraTransforms = {
		FTransform{ FRotator{0.0f, -20.f, 0.0f}, FVector{-1000.0f,0.0f,200.0f }, FVector{1.0f,1.0f,1.0f }},
		FTransform{ FRotator{0.0f, -30.f, 0.0f}, FVector{-500.0f,0.0f,300.0f }, FVector{1.0f,1.0f,1.0f }}
	};
};

UCLASS()
class UNREALVISION_API AUnrealVisionGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	public:
		AUnrealVisionGameMode();

		UFUNCTION(BlueprintCallable, Category = "UnrealVision")
		FString GetMapName();

		UFUNCTION(BlueprintCallable, Category = "UnrealVision")
		TArray<FString> GetSelectableLevelSequences();

		UFUNCTION(BlueprintCallable, Category = "UnrealVision")
		void SetActiveLevelSequence(int index);

		UFUNCTION(BlueprintCallable, Category = "UnrealVision")
		bool GetLevelSequencePlayerPlaybackState();

		UFUNCTION(BlueprintCallable, Category = "UnrealVision")
		bool GetGeneratingDatasetStatus();

		UFUNCTION(BlueprintCallable, Category = "UnrealVision")
		double GetGeneratingDatasetProgress();
		
		UFUNCTION(BlueprintCallable, Category = "UnrealVision")
		void ToggleLevelSequencePlayback();

		UFUNCTION(BlueprintCallable, Category = "UnrealVision")
		void GenerateDataset();

		UFUNCTION(BlueprintCallable, Category = "UnrealVision")
		void CancelGenerateDataset();

		void JumpToLevelSequenceFrame(int32 Frame);

		class ACaptureManager* CaptureManager;
		class AAnnotationManager* AnnotationManager;
		class ALevelSequenceActor* LevelSequenceActor;
		FString CurrentSavePath;
		bool bGeneratingDataset;
		double GeneratingDatasetProgress;
		int CaptureIndex;
	
	protected:
		virtual void BeginPlay() override;
		
	private:
		FUnrealVisionConfig ReadConfigFromJsonFile(const FString &JsonFilePath, bool& bOutSuccess, FString& OutMessage);
		void WriteConfigToJsonFile(const FString &JsonFilePath, bool& bOutSuccess, FString& OutMessage);
		FString ConfigFilePath;
		FString MapName;
		FUnrealVisionConfig	Config;
		TSubclassOf<class UUserWidget> InGameWidgetClass;
		class UUserWidget* InGameWidget;
		bool bCameraStreamEnabled;
		int ActiveLevelSequenceIndex;
		void LoadInGameWidgetClass();
		void CreateInGameWidget();
	    void ResetLevelSequencePlayer();		
		void LoadAvailableLevelSequences();
		void LoadGameModeConfigurations(const FString &ConfigPath);
		void SaveGameModeConfigurations(const FString &ConfigPath);
		TArray<FAssetData> LevelSequenceAssets;
		FString ActiveLevelSequenceName;
		// int CaptureIndex;
		// bool IsRecording;
		// bool IsStreaming;
};
