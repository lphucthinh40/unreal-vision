// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnnotationManager.generated.h"

USTRUCT()
struct FCameraViewInfo{
	GENERATED_BODY()
	FString Name;
	FMinimalViewInfo ViewInfo;
	FCameraViewInfo() : Name(""), ViewInfo() {}
	FCameraViewInfo(FString Name, FMinimalViewInfo ViewInfo) : Name(Name), ViewInfo(ViewInfo) {}
};

UCLASS()
class UNREALVISION_API AAnnotationManager : public AActor
{
	GENERATED_BODY()

	public:
		// Sets default values for this actor's properties
		AAnnotationManager();
		bool bBusy;
	
	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
		TMap<FString, int> MeshSegmentMap;

	private:
		void SetupSegmentAnnotation();
		void LoadLevelAnnotationComponents();
		TArray<class UAnnotationComponent*> AnnotationComps;
		// TArray<FCameraViewInfo> CameraViewInfos;
		TSubclassOf<AActor> BPSkeletalActor_Class;

	public:	
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		UFUNCTION(BlueprintCallable, Category = "Annotate")
		void AnnotateNonBlocking(FString PathName, FString FilePrefix, uint8 Flag);
};
