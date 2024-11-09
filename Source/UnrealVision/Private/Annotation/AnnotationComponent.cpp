// Fill out your copyright notice in the Description page of Project Settings.


#include "Annotation/AnnotationComponent.h"

#include "Animation/SkeletalMeshActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/LineBatchComponent.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UAnnotationComponent::UAnnotationComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UAnnotationComponent::BeginPlay()
{
	Super::BeginPlay();
	Actor = Cast<ASkeletalMeshActor>(GetOwner());
	if (Actor) {
		ActorName = Actor->Tags[0].ToString();
		SkeletalMeshComp = Actor->GetSkeletalMeshComponent();
	}
}

// Called every frame
void UAnnotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// UpdatePoseAnnotation();
	// UpdateBBoxAnnotation();
	// CurrentTimestamp = FDateTime::Now().ToUnixTimestamp();
	// RenderDebugAnnotation();
}

void UAnnotationComponent::UpdatePoseAnnotation() {
	if (SkeletalMeshComp) {
		for (int i=0; i < N_LANDMARKS; i++) {
			OpenPoseLandmarks[i] = SkeletalMeshComp->GetSocketTransform(OpenPoseKeys[i], ERelativeTransformSpace::RTS_World).GetTranslation();
		}
	}
}

void UAnnotationComponent::UpdateBBoxAnnotation()
{
	SkeletalMeshComp->UpdateBounds();
	FVector Origin;
	FVector BoxExtent;
	Actor->GetActorBounds(true, Origin, BoxExtent);

	// Compute the 8 corners of the bounding box
	float ScaleFactor = 0.8; // temporary fix for skeletal mesh with large collision components
	int32 Index = 0;
	for (int32 ZSign = -1; ZSign <= 1; ZSign += 2)
	{
		for (int32 XSign = -1; XSign <= 1; XSign += 2)
		{
			for (int32 YSign = -1; YSign <= 1; YSign += 2)
			{
				FVector Corner = Origin + FVector(XSign * BoxExtent.X * ScaleFactor, YSign * BoxExtent.Y * ScaleFactor, ZSign * BoxExtent.Z);
				BBoxCorners[Index++] = Corner;
			}
		}
	}
	// Query Actor Coordinate & Rotation and compute Transform 
	// FBox BBox = Actor->GetComponentsBoundingBox();
	// origin = BBox.GetCenter();
	// extent = Actor->CalculateComponentsBoundingBoxInLocalSpace().GetExtent();
	// FTransform actorTransform = Actor->GetActorTransform();
	// rotation = actorTransform.GetRotation();
	// euleurRotation = rotation.Euler();
	// scale = actorTransform.GetScale3D();
	// transform = FTransform(rotation, origin, scale);
	//
	// // Find 8 Corners of 3D BBox
	// float rescaleFactor = 1.f;
	// BBoxCorners[0] = origin + rotation.RotateVector(FVector(extent.X, extent.Y, extent.Z * rescaleFactor));
	// BBoxCorners[1] = origin + rotation.RotateVector(FVector(extent.X, -extent.Y, extent.Z * rescaleFactor));
	// BBoxCorners[2] = origin + rotation.RotateVector(FVector(-extent.X, extent.Y, extent.Z * rescaleFactor));
	// BBoxCorners[3] = origin + rotation.RotateVector(FVector(-extent.X, -extent.Y, extent.Z * rescaleFactor));
	// BBoxCorners[4] = origin + rotation.RotateVector(FVector(extent.X, extent.Y, -extent.Z * rescaleFactor));
	// BBoxCorners[5] = origin + rotation.RotateVector(FVector(extent.X, -extent.Y, -extent.Z * rescaleFactor));
	// BBoxCorners[6] = origin + rotation.RotateVector(FVector(-extent.X, extent.Y, -extent.Z * rescaleFactor));
	// BBoxCorners[7] = origin + rotation.RotateVector(FVector(-extent.X, -extent.Y, -extent.Z * rescaleFactor));
}

void UAnnotationComponent::RenderDebugAnnotation() {
	if (CurrentTimestamp > 0) {
		if (ShowDebugOpenPoseAnnotation) {
			ULineBatchComponent* const LineBatcher = GetWorld()->ForegroundLineBatcher;
			TArray<FBatchedLine> lineBatch;
			for (TTuple<int,int,FColor> Line : OpenPoseDebugLines) {
				lineBatch.Add(FBatchedLine(OpenPoseLandmarks[Line.Get<0>()], OpenPoseLandmarks[Line.Get<1>()], Line.Get<2>(), 0.019, 2, 0));
			}
			LineBatcher->DrawLines(lineBatch);
		}
		if (ShowDebugBBoxAnnotation) {
			DrawDebugBox(Actor->GetWorld(), origin, extent*0.8, rotation, FColor::Red, false, 0, 0, 1.0f);
		}		
	}
}
