// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnnotationComponent.generated.h"

#define N_LANDMARKS 18
#define N_LINES 17
#define N_BBOX_CORNERS 8

class ASkeletalMeshActor;
class USkeletalMeshComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREALVISION_API UAnnotationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnnotationComponent();
	const FName OpenPoseKeys[N_LANDMARKS] = {
		TEXT("Nose"), TEXT("Neck"),
		TEXT("RShoulder"), TEXT("RElbow"), TEXT("RWrist"),
		TEXT("LShoulder"), TEXT("LElbow"), TEXT("LWrist"),
		TEXT("RHip"), TEXT("RKnee"), TEXT("RAnkle"),
		TEXT("LHip"), TEXT("LKnee"), TEXT("LAnkle"),
		TEXT("REye"), TEXT("LEye"),
		TEXT("REar"), TEXT("LEar")		
	};
	const TTuple<int,int,FColor> OpenPoseDebugLines[N_LINES] = {
		TTuple<int,int,FColor>(0,1, FColor(0,0,153)), 
		TTuple<int,int,FColor>(0,14, FColor(51,0,153)), TTuple<int,int,FColor>(0,15, FColor(153,0,153)),
		TTuple<int,int,FColor>(14,16, FColor(102,0,153)), TTuple<int,int,FColor>(15,17, FColor(153,0,102)),
		TTuple<int,int,FColor>(1,2, FColor(153,0,0)), TTuple<int,int,FColor>(2,3, FColor(153,102,0)), TTuple<int,int,FColor>(3,4, FColor(153,153,0)),
		TTuple<int,int,FColor>(1,5, FColor(153,51,0)), TTuple<int,int,FColor>(5,6, FColor(102,153,0)), TTuple<int,int,FColor>(6,7, FColor(51,153,0)),
		TTuple<int,int,FColor>(1,8, FColor(0,153,0)), TTuple<int,int,FColor>(8,9, FColor(0,153,51)), TTuple<int,int,FColor>(9,10, FColor(0,153,102)),
		TTuple<int,int,FColor>(1,11, FColor(0,153,153)), TTuple<int,int,FColor>(11,12, FColor(0,102,153)), TTuple<int,int,FColor>(12,13, FColor(0,51,153))
	};
	FVector OpenPoseLandmarks[N_LANDMARKS];
	FVector BBoxCorners[N_BBOX_CORNERS];
	FString ActorName;
	
protected:
	ASkeletalMeshActor* Actor;
	USkeletalMeshComponent* SkeletalMeshComp;

	FVector origin;
	FVector extent;
	FQuat rotation;
	FVector scale;

	FVector euleurRotation;
	FVector relativeEuleurRotation;
	FQuat relativeRotation;
	FTransform transform;

	UPROPERTY(EditAnywhere, Category = "Annotation")
	bool ShowDebugOpenPoseAnnotation = false;
	
	UPROPERTY(EditAnywhere, Category = "Annotation")
	bool ShowDebugBBoxAnnotation = false;

	FTimerHandle AnnotationHandler;
	float UpdateRate = 0.03;
	int64 CurrentTimestamp = 0;



	// Called when the game starts
	virtual void BeginPlay() override;

	void RenderDebugAnnotation();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UpdatePoseAnnotation();
	void UpdateBBoxAnnotation();
};