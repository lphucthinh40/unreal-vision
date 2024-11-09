// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/MonoCameraActor.h"
#include "Misc/FileHelper.h"
#include "CaptureManager.generated.h"

USTRUCT()
struct FCaptureRequest{
	GENERATED_BODY()
	TMap<FString, FFusionImageData> ImageData;
	double Timestamp;
	FString CaptureId;
	FString SavePath;
	uint8 CaptureFlag;
	FRenderCommandFence RenderFence;
	FCaptureRequest(){
		CaptureId = TEXT("");
		SavePath = FPaths::ProjectSavedDir();
		CaptureFlag =  0x0F;
	}
	FCaptureRequest(FString Id, FString Path, uint8 Flag){
		CaptureId = Id;
		SavePath = Path;
		CaptureFlag = Flag;
	}
};

UCLASS()
class UNREALVISION_API ACaptureManager : public AActor
{
	GENERATED_BODY()

	public:
		// Sets default values for this actor's properties
		ACaptureManager();

		// Raw Image Capture  Component
		TArray<class AMonoCameraActor*> MonocularCameraActors;
        bool bBusy;

		void SaveAllCameraMetadataToFile(FString PathName, FString FilePrefix);
	
	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Queue for handling Capture Requests
		TQueue<FCaptureRequest*> CaptureRequestQueue;

		// Async task for saving the captured image to disk
		void RunAsyncImageSaveTask(TArray64<uint8> Image, FString ImageName);
		void SaveFusionImageDataToFile(FFusionImageData* ImageData, FString PathName, FString FilePrefix, uint8 Flag);

	public:	
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		UFUNCTION(BlueprintCallable, Category = "CaptureImages")
		void CaptureNonBlocking(FString CaptureId, FString SavePath, uint8 Flag);
};
