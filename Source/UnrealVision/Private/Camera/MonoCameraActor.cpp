// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/MonoCameraActor.h"

#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Camera/ColorCamera.h"
#include "Camera/DepthCamera.h"
#include "Camera/SegmentCamera.h"
#include "Camera/NormalCamera.h"
#include "Kismet/GameplayStatics.h"

AMonoCameraActor::AMonoCameraActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	FString ComponentName;
	ComponentName = FString::Printf(TEXT("%s_%s"), *this->GetName(), TEXT("Preview"));
	PreviewCamera = CreateDefaultSubobject<UCameraComponent>(*ComponentName);
	PreviewCamera->SetupAttachment(RootComponent);
	
	ComponentName = FString::Printf(TEXT("%s_%s"), *this->GetName(), TEXT("Color"));
	ColorCamera = CreateDefaultSubobject<UColorCamera>(*ComponentName);
	ColorCamera->SetupAttachment(RootComponent);
	Sensors.Add(ColorCamera);
	
	ComponentName = FString::Printf(TEXT("%s_%s"), *this->GetName(), TEXT("Segment"));
	SegmentCamera = CreateDefaultSubobject<USegmentCamera>(*ComponentName);
	SegmentCamera->SetupAttachment(RootComponent);
	Sensors.Add(SegmentCamera);
	
    ComponentName = FString::Printf(TEXT("%s_%s"), *this->GetName(), TEXT("Depth"));
	DepthCamera = CreateDefaultSubobject<UDepthCamera>(*ComponentName);
	DepthCamera->SetupAttachment(RootComponent);
	Sensors.Add(DepthCamera);
	
	ComponentName = FString::Printf(TEXT("%s_%s"), *this->GetName(), TEXT("Normal"));
	NormalCamera = CreateDefaultSubobject<UNormalCamera>(*ComponentName);
	NormalCamera->SetupAttachment(RootComponent);
	Sensors.Add(NormalCamera);
	
	ImageWidth = 1920;
	ImageHeight = 1080;
	FOV = 90;
}

void AMonoCameraActor::BeginPlay()
{
	Super::BeginPlay();
	SetFOV(FOV);
	SetImageSize(ImageWidth, ImageHeight);
	UpdateCameraMatrices();
}

void AMonoCameraActor::SetImageSize(int Width, int Height)
{
	ImageWidth = Width;
	ImageHeight = Height;
    for (UBaseCamera* Sensor : Sensors)	{
		Sensor->SetImageSize(ImageWidth, ImageHeight);
	}
}

void AMonoCameraActor::SetFOV(float fov)
{
	FOV = fov;
	for (UBaseCamera* Sensor : Sensors) {
		Sensor->SetFOV(fov);
	}
}

FMinimalViewInfo AMonoCameraActor::GetViewInfo() {
	FMinimalViewInfo ViewInfo;
	ColorCamera->GetCameraView(0.0f, ViewInfo);
	return ViewInfo;
}

void AMonoCameraActor::SetLocation(FVector Location) {
	this->SetActorLocation(Location);
}

void AMonoCameraActor::SetRotation(FRotator Rotator) {
	this->SetActorRotation(Rotator);
}

void AMonoCameraActor::GenerateRenderRequests(FFusionImageData* ImageData, uint8 Flag) {
	if (Flag & CAPTURE_COLOR) {
		// UE_LOG(LogTemp, Warning, TEXT("COLOR: %f, %f, %f"), ColorCamera->GetLocation().X, ColorCamera->GetLocation().Y, ColorCamera->GetLocation().Z);
		ColorCamera->CaptureScene();
		FTextureRenderTargetResource* renderTargetResource = ColorCamera->TextureTarget->GameThread_GetRenderTargetResource();
		FReadSurfaceContext readSurfaceContext = {
			renderTargetResource,
			&(ImageData->Color),
			FIntRect(0,0,renderTargetResource->GetSizeXY().X, renderTargetResource->GetSizeXY().Y),
			FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
		};
		ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
			[readSurfaceContext](FRHICommandListImmediate& RHICmdList){
				RHICmdList.ReadSurfaceData(
					readSurfaceContext.SrcRenderTarget->GetRenderTargetTexture(),
					readSurfaceContext.Rect,
					*readSurfaceContext.OutData,
					readSurfaceContext.Flags
				);
		});
	}
	if (Flag & CAPTURE_SEGMENT) {
		SegmentCamera->CaptureScene();
		FTextureRenderTargetResource* renderTargetResource = SegmentCamera->TextureTarget->GameThread_GetRenderTargetResource();
		FReadSurfaceContext readSurfaceContext = {
			renderTargetResource,
			&ImageData->Segment,
			FIntRect(0,0,renderTargetResource->GetSizeXY().X, renderTargetResource->GetSizeXY().Y),
			FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
		};
		ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
			[readSurfaceContext](FRHICommandListImmediate& RHICmdList){
				RHICmdList.ReadSurfaceData(
					readSurfaceContext.SrcRenderTarget->GetRenderTargetTexture(),
					readSurfaceContext.Rect,
					*readSurfaceContext.OutData,
					readSurfaceContext.Flags
				);
		});			
	}
	if (Flag & CAPTURE_NORMAL) {
		NormalCamera->CaptureScene();
		FTextureRenderTargetResource* renderTargetResource = NormalCamera->TextureTarget->GameThread_GetRenderTargetResource();
		FReadSurfaceContext readSurfaceContext = {
			renderTargetResource,
			&(ImageData->Normal),
			FIntRect(0,0,renderTargetResource->GetSizeXY().X, renderTargetResource->GetSizeXY().Y),
			FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
		};
		ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
			[readSurfaceContext](FRHICommandListImmediate& RHICmdList){
				RHICmdList.ReadSurfaceData(
					readSurfaceContext.SrcRenderTarget->GetRenderTargetTexture(),
					readSurfaceContext.Rect,
					*readSurfaceContext.OutData,
					readSurfaceContext.Flags
				);
		});			
	}
	if (Flag & CAPTURE_DEPTH) {
		DepthCamera->CaptureScene();
		FTextureRenderTargetResource* renderTargetResource = DepthCamera->TextureTarget->GameThread_GetRenderTargetResource();
		FReadSurfaceFloatContext readSurfaceContext = {
			renderTargetResource,
			&ImageData->Depth,
			FIntRect(0, 0, renderTargetResource->GetSizeXY().X, renderTargetResource->GetSizeXY().Y),
			FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
		};
		ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
			[readSurfaceContext](FRHICommandListImmediate& RHICmdList){
			RHICmdList.ReadSurfaceFloatData(
				readSurfaceContext.SrcRenderTarget->GetRenderTargetTexture(),
				readSurfaceContext.Rect,
				*readSurfaceContext.OutData,
				readSurfaceContext.Flags
			);
		});
	}
}

void AMonoCameraActor::UpdateCameraMatrices() {
	FMinimalViewInfo MinimalViewInfo;
	ColorCamera->GetCameraView(0.f, MinimalViewInfo);
	UE_LOG(LogTemp, Error, TEXT("MINIMALVIEWINFO: FOV(%f), Position(%f,%f,%f), Rotation(%f,%f,%f)"), MinimalViewInfo.DesiredFOV, MinimalViewInfo.Location.X, MinimalViewInfo.Location.Y, MinimalViewInfo.Location.Z, MinimalViewInfo.Rotation.Roll, MinimalViewInfo.Rotation.Pitch, MinimalViewInfo.Rotation.Yaw);
	UE_LOG(LogTemp, Error, TEXT("MINIMALVIEWINFO: RATIO(%f), %d"), MinimalViewInfo.AspectRatio, MinimalViewInfo.ProjectionMode == ECameraProjectionMode::Perspective);
	TOptional<FMatrix> CustomProjectionMatrix;
	if (ColorCamera->bUseCustomProjectionMatrix)	{
		CustomProjectionMatrix = ColorCamera->CustomProjectionMatrix;
	}
	UGameplayStatics::CalculateViewProjectionMatricesFromMinimalView(
		MinimalViewInfo,
		CustomProjectionMatrix,
		ViewMatrix,
		ProjectionMatrix,
		ViewProjectionMatrix
	);
	// Define the render target rectangle
	FIntRect RenderTargetSize(0,0,1920, 1080);
	FVector WorldPoint1 = FVector(45.742018,0.760089,143.2736);
	FVector WorldPoint2 = FVector(19.340385,-8.220062,12.385978);
	FVector2D ScenePoint1, ScenePoint2;
	// Compute the 2D screen coordinates using FSceneView::ProjectWorldToScreen
	FVector2D ScreenCoordinates;
	FSceneView::ProjectWorldToScreen(
		WorldPoint1,
		RenderTargetSize,
		ViewProjectionMatrix,
		ScenePoint1
	);
	FSceneView::ProjectWorldToScreen(
		WorldPoint2,
		RenderTargetSize,
		ViewProjectionMatrix,
		ScenePoint2
	);
	FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPoint1, 1.f));
	UE_LOG(LogTemp, Error, TEXT("PROJECTION MATRIX: (%f, %f, %f)"), ViewProjectionMatrix.M[0][0], ViewProjectionMatrix.M[0][1], ViewProjectionMatrix.M[0][2]);
	UE_LOG(LogTemp, Error, TEXT("RESULT PROJECTION: (%f, %f)-(%f, %f)"), ScenePoint1.X, ScenePoint1.Y, ScenePoint2.X, ScenePoint2.Y);
	UE_LOG(LogTemp, Error, TEXT("RESULT: (%f, %f, %f, %f)"), Result.X, Result.Y, Result.Z, Result.W);
}
