// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCamera.h"
#include "NormalCamera.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREALVISION_API UNormalCamera : public UBaseCamera
{
	GENERATED_BODY()

public:
	UNormalCamera(const FObjectInitializer& ObjectInitializer);
};
