// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorCamera.h"
#include "GameFramework/Actor.h"
#include "MonoCameraActor.generated.h"

#define CAPTURE_COLOR   1
#define CAPTURE_SEGMENT 2
#define CAPTURE_NORMAL  4
#define CAPTURE_DEPTH   8

USTRUCT()
struct FFusionImageData{
	GENERATED_BODY()
	TArray<FColor> Color;
	TArray<FColor> Segment;
	TArray<FColor> Normal;
	TArray<FFloat16Color> Depth;
	int ImageHeight;
	int ImageWidth;
	FFusionImageData(){
		ImageHeight = 1080;
		ImageWidth =  1920;
	}
	FFusionImageData(int Width, int Height){
		ImageHeight = Height;
		ImageWidth =  Width;
	}
};

struct FReadSurfaceContext{
	FRenderTarget* SrcRenderTarget;
	TArray<FColor>* OutData;
	FIntRect Rect;
	FReadSurfaceDataFlags Flags;
};

struct FReadSurfaceFloatContext{
	FRenderTarget* SrcRenderTarget;
	TArray<FFloat16Color>* OutData;
	FIntRect Rect;
	FReadSurfaceDataFlags Flags;
};

UCLASS()
class UNREALVISION_API AMonoCameraActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMonoCameraActor(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	UPROPERTY(meta = (AllowPrivateAccess= "true"))
	class UCameraComponent* PreviewCamera;
	class UColorCamera* ColorCamera;
	class USegmentCamera* SegmentCamera;
	class UDepthCamera* DepthCamera;
	class UNormalCamera* NormalCamera;
	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;
	FMatrix ViewProjectionMatrix;
	
	void SetImageSize(int Width, int Height);
	int GetImageHeight() { return ImageHeight; }
	int GetImageWidth()  { return ImageWidth;  }
	void SetFOV(float fov);
	float GetFOV()		 { return FOV;         }
	FMinimalViewInfo GetViewInfo();
	void SetLocation(FVector Location);
	void SetRotation(FRotator Rotator);
	void GenerateRenderRequests(FFusionImageData* ImageData, uint8 Flag);
	void ConvertPointsFromWorldToPixel(TArray<FVector>* WorldPoints);
	
protected:
	TArray<class UBaseCamera*> Sensors;

	
private:
	int ImageWidth;
	int ImageHeight;
	float FOV;
	void UpdateCameraMatrices();

};
