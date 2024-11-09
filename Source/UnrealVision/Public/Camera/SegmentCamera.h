// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCamera.h"
#include "SegmentCamera.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREALVISION_API USegmentCamera : public UBaseCamera
{
	GENERATED_BODY()

public:
	USegmentCamera(const FObjectInitializer& ObjectInitializer);
};
