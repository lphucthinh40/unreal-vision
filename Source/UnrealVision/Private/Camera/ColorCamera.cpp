// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ColorCamera.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/EngineTypes.h"

UColorCamera::UColorCamera(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	PostProcessSettings.bOverride_DynamicGlobalIlluminationMethod = 1;
	PostProcessSettings.bOverride_ReflectionMethod = 1;
	PostProcessSettings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::Lumen;
	PostProcessSettings.ReflectionMethod = EReflectionMethod::Lumen;
};

void UColorCamera::SetupTextureTarget(int Width, int Height)
{
	Super::SetupTextureTarget(Width, Height);
}

void UColorCamera::Capture(TArray<FColor>& Image) {
	this->CaptureScene();
	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); 
	TextureTarget->GameThread_GetRenderTargetResource()->ReadPixels(Image, ReadSurfaceDataFlags);
}