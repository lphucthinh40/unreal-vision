// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCamera.h"
#include "ColorCamera.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREALVISION_API UColorCamera : public UBaseCamera
{
	GENERATED_BODY()

	public:
		// Sets default values for this component's properties
		UColorCamera(const FObjectInitializer& ObjectInitializer);
		virtual void SetupTextureTarget(int Width, int Height) override;
		void Capture(TArray<FColor>& Image);
};
