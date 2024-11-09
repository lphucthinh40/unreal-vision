// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "BaseCamera.generated.h"

/**
 * 
 */
UCLASS(abstract)
class UNREALVISION_API UBaseCamera : public USceneCaptureComponent2D
{
	GENERATED_BODY()

	public:
		UBaseCamera(const FObjectInitializer& ObjectInitializer);
	
		/** Capture API */
		FTextureRenderTargetResource* GetRenderTargetResource(); // non-blocking version
		void Capture(TArray<FColor>& ImageData);
		void CaptureToFile(FString &CaptureId, FString &SavePath);

		/** Initialization */
		virtual void SetupTextureTarget(int Width, int Height);
		void SetPostProcessMaterial(UMaterial* PostProcessMaterial); // needed for Normal View
			
		/** Projection Matrix */
		FString GetProjectionMatrix();
		void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView);

		/** Basic Set/Get for Camera Properties */		
		float GetFOV()                            { return this->FOVAngle; }
		void SetFOV(float FOV)                    { this->FOVAngle = FOV; }		
		FVector GetLocation()                     { return this->GetComponentLocation(); }
		void SetLocation(FVector Location)        { this->SetWorldLocation(Location); }
		FRotator GetRotation()                    { return this->GetComponentRotation(); }
		void SetRotation(FRotator Rotator)        { this->SetWorldRotation(Rotator);}		
		void SetImageSize(int Width, int Height)  { this->ImageWidth=Width; this->ImageHeight=Height; SetupTextureTarget(Width, Height); }
		int GetImageWidth()                       { return this->ImageWidth; }
		int GetImageHeight()					  { return this->ImageHeight; }

	protected:

	bool CheckTextureTarget();
	int ImageWidth;
	int ImageHeight;
};