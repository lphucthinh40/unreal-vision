// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "UnrealVisionPawn.generated.h"

UCLASS()
class UNREALVISION_API AUnrealVisionPawn : public ADefaultPawn
{
	GENERATED_BODY()

	public:
		AUnrealVisionPawn();
		virtual void BeginPlay() override;
		virtual void Tick( float DeltaSeconds ) override;
		virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	private:
		class AUnrealVisionGameMode* GameMode;
		class UMonoCameraActor* Camera;
		bool LookAroundEnabled;
		int WarmUpCounter;
		int WarmUpThreshold;
		void HandleLookUp(float Val);
		void HandleLookRight(float Val);	
		void OnEnableLookAround();
		void OnDisableLookAround();
};
