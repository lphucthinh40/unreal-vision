// Fill out your copyright notice in the Description page of Project Settings.

#include "CaptureManager.h"
#include "Camera/MonoCameraActor.h"
#include "Utils/Serialization.h"
#include "Tasks/SaveImageToDisk.h"

#include "Kismet/GameplayStatics.h"
#include "IImageWrapperModule.h"

// Sets default values
ACaptureManager::ACaptureManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bBusy = false;
}

// Called when the game starts or when spawned
void ACaptureManager::BeginPlay()
{
	Super::BeginPlay();

	// Find all annotation cameras
	TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonoCameraActor::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0) {		
		for (int32 Idx = 0; Idx < FoundActors.Num(); Idx++) {
			AMonoCameraActor* CameraActor = Cast<AMonoCameraActor>(FoundActors[Idx]);
			MonocularCameraActors.Add(CameraActor);
    	}
		UE_LOG(LogTemp, Warning, TEXT("Found %d Camera Actors for this level"), FoundActors.Num());
	} else {
		UE_LOG(LogTemp, Error, TEXT("No Camera Actors found!"));
	}	
}

// Called every frame
void ACaptureManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	if(!CaptureRequestQueue.IsEmpty()){
		// Peek the next RenderRequest from queue
		FCaptureRequest* nextCaptureRequest = nullptr;
		CaptureRequestQueue.Peek(nextCaptureRequest);

		if(nextCaptureRequest){ //nullptr check
			if(nextCaptureRequest->RenderFence.IsFenceComplete()){ // Check if rendering is done, indicated by RenderFence
				// Load the image wrapper module 
				UE_LOG(LogTemp, Warning, TEXT("Processing Current Capture Request"));

				for (auto it = nextCaptureRequest->ImageData.CreateIterator(); it; ++it) {
					FString PathName = nextCaptureRequest->SavePath + "/Captures/" + it->Key;
					SaveFusionImageDataToFile(&(it->Value), PathName, nextCaptureRequest->CaptureId, nextCaptureRequest->CaptureFlag);
				}
				
				// Delete the first element from RenderQueue
				CaptureRequestQueue.Pop();
				delete nextCaptureRequest;
			}
		}
	}
}

void ACaptureManager::RunAsyncImageSaveTask(TArray64<uint8> Image, FString ImageName) {
	 UE_LOG(LogTemp, Warning, TEXT("Running Async Task"));
    (new FAutoDeleteAsyncTask<AsyncSaveImageToDiskTask>(Image, ImageName))->StartBackgroundTask();
}


void ACaptureManager::CaptureNonBlocking(FString CaptureId, FString SavePath, uint8 Flag) {
	if(MonocularCameraActors.Num() < 1){
        UE_LOG(LogTemp, Error, TEXT("CaptureNonBlocking: No valid Camera Actors found for this level!"));
        return;
    } else {
		UE_LOG(LogTemp, Warning, TEXT("Start Non-Blocking Image Capture"));

		// Init new RenderRequest
    	FCaptureRequest* captureRequest = new FCaptureRequest(CaptureId, SavePath, Flag);
		for (int32 Idx = 0; Idx < MonocularCameraActors.Num(); Idx++) {
			FString CamId = MonocularCameraActors[Idx]->GetActorNameOrLabel();
			FFusionImageData ImageData;
			captureRequest->ImageData.Add(CamId, ImageData);
			MonocularCameraActors[Idx]->GenerateRenderRequests(&(captureRequest->ImageData[CamId]), Flag);
    	}

		UE_LOG(LogTemp, Warning, TEXT("All Render Requests Have Been Sent. Enqueue Capture Request"));
		// Notifiy new task in RenderQueue
		CaptureRequestQueue.Enqueue(captureRequest);

		// Set RenderCommandFence
		captureRequest->RenderFence.BeginFence();
	}
}

void ACaptureManager::SaveFusionImageDataToFile(FFusionImageData* ImageData, FString PathName, FString FilePrefix, uint8 Flag) {
	bBusy = true;
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	if (Flag & CAPTURE_COLOR) {
		FString fileName = PathName + "/Color/" + FilePrefix + ".png";
		static TSharedPtr<IImageWrapper> imageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
		imageWrapper->SetRaw((ImageData->Color).GetData(), (ImageData->Color).GetAllocatedSize(), ImageData->ImageWidth, ImageData->ImageHeight, ERGBFormat::BGRA, 8);
		const TArray64<uint8>& ImgData = imageWrapper->GetCompressed(5);
		// RunAsyncImageSaveTask(ImgData, fileName);
		FFileHelper::SaveArrayToFile(ImgData, *fileName);
	}
	if (Flag & CAPTURE_SEGMENT) {
		FString fileName = PathName + "/Segment/" + FilePrefix + ".png";
		for (FColor& Pixel : ImageData->Segment)
		{ Pixel.A = 255; }
		static TSharedPtr<IImageWrapper> imageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
		imageWrapper->SetRaw((ImageData->Segment).GetData(), (ImageData->Segment).GetAllocatedSize(), ImageData->ImageWidth, ImageData->ImageHeight, ERGBFormat::BGRA, 8);
		const TArray64<uint8>& ImgData = imageWrapper->GetCompressed(5);
		FFileHelper::SaveArrayToFile(ImgData, *fileName);
	}
	if (Flag & CAPTURE_NORMAL) {
		FString fileName = PathName + "/Normal/" + FilePrefix + ".png";
		for (FColor& Pixel : ImageData->Normal)
		{ Pixel.A = 255; }
		static TSharedPtr<IImageWrapper> imageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
		imageWrapper->SetRaw((ImageData->Normal).GetData(), (ImageData->Normal).GetAllocatedSize(), ImageData->ImageWidth, ImageData->ImageHeight, ERGBFormat::BGRA, 8);
		const TArray64<uint8>& ImgData = imageWrapper->GetCompressed(5);
		FFileHelper::SaveArrayToFile(ImgData, *fileName);
	}
	if (Flag & CAPTURE_DEPTH) {
		FString fileName = PathName + "/Depth/" + FilePrefix + ".npy";
		TArray<FFloat16> DepthData;
		DepthData.AddUninitialized((ImageData->Depth).Num());
		for (int i=0; i<(ImageData->Depth).Num(); i++)	{
			DepthData[i] = ImageData->Depth[i].R;
		}
		const TArray64<uint8>& BinaryData = FSerialization::Array2DToNpy(DepthData, ImageData->ImageWidth, ImageData->ImageHeight);
		FFileHelper::SaveArrayToFile(BinaryData, *fileName);
	}
	bBusy = false;
}

void ACaptureManager::SaveAllCameraMetadataToFile(FString PathName, FString FilePrefix) {
	if(MonocularCameraActors.Num() > 0) {
		FString FilePath = PathName + "/Annotations/" + FilePrefix + ".csv";
		FString Data = "Id,Name,ImageHeight,ImageWidth,FOV,Transform,Rotation,ViewMatrix,ProjectionMatrix,ViewProjectionMatrix\n";
		for (int32 Idx = 0; Idx < MonocularCameraActors.Num(); Idx++) {
			FVector Location = MonocularCameraActors[Idx]->ColorCamera->GetComponentLocation();
			FRotator Rotation = MonocularCameraActors[Idx]->ColorCamera->GetComponentRotation();
			// General Data
			Data +=
				FString::FromInt(Idx) + "," +
					MonocularCameraActors[Idx]->GetActorNameOrLabel() + "," +
						FString::FromInt(MonocularCameraActors[Idx]->GetImageHeight()) + "," +
							FString::FromInt(MonocularCameraActors[Idx]->GetImageWidth()) + "," +
								FString::SanitizeFloat(MonocularCameraActors[Idx]->GetFOV()) + "," +
									"\"[" + FString::SanitizeFloat(Location.X) + "," + FString::SanitizeFloat(Location.Y) + "," + FString::SanitizeFloat(Location.Z) + "]\"," +
										"\"[" + FString::SanitizeFloat(Rotation.Roll) + "," + FString::SanitizeFloat(Rotation.Pitch) + "," + FString::SanitizeFloat(Rotation.Yaw) + "]\",";
			// ViewMatrix
			Data += "\"[";
			for (int32 Row = 0; Row < 4; ++Row)	{
				Data += "[";
				for (int32 Col = 0; Col < 4; ++Col) {
					Data += FString::SanitizeFloat(MonocularCameraActors[Idx]->ViewMatrix.M[Row][Col]);
					Data += Col<3 ? "," : "";
				}
				Data += Row<3 ? "]," : "]";
			}
			Data += "]\",";
			// ProjectionMatrix
            			Data += "\"[";
            			for (int32 Row = 0; Row < 4; ++Row)	{
            				Data += "[";
            				for (int32 Col = 0; Col < 4; ++Col) {
            					Data += FString::SanitizeFloat(MonocularCameraActors[Idx]->ProjectionMatrix.M[Row][Col]);
            					Data += Col<3 ? "," : "";
            				}
            				Data += Row<3 ? "]," : "]";
            			}
            			Data += "]\",";
			// ViewProjectionMatrix
			Data += "\"[";
			for (int32 Row = 0; Row < 4; ++Row)	{
				Data += "[";
				for (int32 Col = 0; Col < 4; ++Col) {
					Data += FString::SanitizeFloat(MonocularCameraActors[Idx]->ViewProjectionMatrix.M[Row][Col]);
					Data += Col<3 ? "," : "";
				}
				Data += Row<3 ? "]," : "]";
			}
			Data += "]\"\n";
		}
		FFileHelper::SaveStringToFile(Data, *FilePath);
	}
}