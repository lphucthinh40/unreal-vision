// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCamera.h"
#include "DepthCamera.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREALVISION_API UDepthCamera : public UBaseCamera
{
	GENERATED_BODY()

public:
	UDepthCamera(const FObjectInitializer& ObjectInitializer);
	void Capture(TArray<float>& DepthData);
	virtual void SetupTextureTarget(int Width, int Height) override;
};
