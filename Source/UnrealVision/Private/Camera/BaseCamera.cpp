// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/BaseCamera.h"

#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ShowFlags.h"

UBaseCamera::UBaseCamera(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	this->ShowFlags.SetPostProcessing(true);
	this->ShowFlags.SetPostProcessMaterial(true);
    this->ShowFlags.SetTemporalAA(true);
	bCaptureEveryFrame = false;
	bCaptureOnMovement = false;
  	bAlwaysPersistRenderingState = true;
	CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	ImageWidth = 1920;
	ImageHeight = 1080;
	FOVAngle = 90;
}

void UBaseCamera::SetupTextureTarget(int Width, int Height)
{
	CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Width, Height, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor(0, 0, 0), true, false);    // TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
    TextureTarget->bGPUSharedFlag = true; // demand buffer on GPU
	TextureTarget->TargetGamma = GEngine->GetDisplayGamma();
	ShowFlags.SetTemporalAA(true);
}

void UBaseCamera::CaptureToFile(FString &CaptureId, FString &SavePath) {
	FString FileName = CaptureId + ".png";
	UKismetRenderingLibrary::ExportRenderTarget(GetWorld(), TextureTarget, SavePath, FileName);
	UE_LOG(LogTemp, Error, TEXT("SAVE TO FILE!"));
}

bool UBaseCamera::CheckTextureTarget()
{
	return IsValid(TextureTarget) && TextureTarget->SizeX != 0 && TextureTarget->SizeY != 0;
}

void UBaseCamera::SetPostProcessMaterial(UMaterial* PostProcessMaterial)
{
	AddOrUpdateBlendable(PostProcessMaterial, 1);
}

void UBaseCamera::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	DesiredView.Location = GetComponentLocation();
	DesiredView.Rotation = GetComponentRotation();
	DesiredView.AspectRatio = (float)ImageWidth / (float)ImageHeight;
	DesiredView.FOV = this->FOVAngle;
	DesiredView.ProjectionMode = ECameraProjectionMode::Perspective;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.0f)	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}
}

FTextureRenderTargetResource* UBaseCamera::GetRenderTargetResource() {
    return TextureTarget->GameThread_GetRenderTargetResource();
}