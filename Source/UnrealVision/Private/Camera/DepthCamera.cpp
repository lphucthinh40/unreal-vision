// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/DepthCamera.h"
#include "Engine/TextureRenderTarget2D.h"

UDepthCamera::UDepthCamera(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	this->CaptureSource = ESceneCaptureSource::SCS_SceneDepth;
}

void UDepthCamera::SetupTextureTarget(int Width, int Height)
{
	TextureTarget = NewObject<UTextureRenderTarget2D>(this);
	TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
	EPixelFormat PixelFormat = EPixelFormat::PF_FloatRGBA;
	bool bUseLinearGamma = true;
	TextureTarget->InitCustomFormat(Width, Height, PixelFormat, bUseLinearGamma);
	TextureTarget->bGPUSharedFlag = true; // demand buffer on GPU
	TextureTarget->TargetGamma = GEngine->GetDisplayGamma();
	ShowFlags.SetTemporalAA(true);
}

void UDepthCamera::Capture(TArray<float>& DepthData)
{
	this->CaptureScene();
	DepthData.AddZeroed(ImageWidth * ImageHeight); // or AddUninitialized(FloatColorDepthData.Num());
	FTextureRenderTargetResource* RenderTargetResource = TextureTarget->GameThread_GetRenderTargetResource();
	TArray<FFloat16Color> FloatColorDepthData;
	RenderTargetResource->ReadFloat16Pixels(FloatColorDepthData);

	for (int i = 0; i < FloatColorDepthData.Num(); i++)
	{
		FFloat16Color& FloatColor = FloatColorDepthData[i];
		DepthData[i] = FloatColor.R;
	}
}
