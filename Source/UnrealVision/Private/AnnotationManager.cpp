// Fill out your copyright notice in the Description page of Project Settings.

#include "AnnotationManager.h"
#include "Annotation/AnnotationComponent.h"
#include "Misc/FileHelper.h"
#include "Utils/Helpers.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/SkeletalMeshActor.h"
#include "Camera/MonoCameraActor.h"

// Sets default values
AAnnotationManager::AAnnotationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FClassFinder<AActor> SkeletalActorBPClass(TEXT("Blueprint'/Game/Blueprints/BP_SkeletalMeshActor.BP_SkeletalMeshActor_C'"));
	BPSkeletalActor_Class = SkeletalActorBPClass.Class;
	bBusy = false;
}

// Called when the game starts or when spawned
void AAnnotationManager::BeginPlay()
{
	Super::BeginPlay();

	// Find all annotation cameras and extract view info
	// TArray<AActor*> FoundActors;
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonoCameraActor::StaticClass(), FoundActors);
	// if (FoundActors.Num() > 0) {		
	// 	for (int32 Idx = 0; Idx < FoundActors.Num(); Idx++) {
	// 		AMonoCameraActor* CameraActor = Cast<AMonoCameraActor>(FoundActors[Idx]);
	// 		FCameraViewInfo ViewInfo = FCameraViewInfo(CameraActor->GetActorNameOrLabel(), CameraActor->GetViewInfo());
	// 		CameraViewInfos.Add(ViewInfo);
	// 	}
	// 	UE_LOG(LogTemp, Warning, TEXT("Found %d Camera Actors for this level"), FoundActors.Num());
	// } else {
	// 	UE_LOG(LogTemp, Error, TEXT("No Camera Actors found!"));
	// }
}

// Called every frame
void AAnnotationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (AnnotationComps.Num() == 0) {
		LoadLevelAnnotationComponents();
		SetupSegmentAnnotation();
	}
}

void AAnnotationManager::LoadLevelAnnotationComponents() {
	TArray<AActor*> Actors;
	// Get Capture Manager
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASkeletalMeshActor::StaticClass(), Actors);
    UE_LOG(LogTemp, Error, TEXT("Found %d Actors for this level"), Actors.Num());
	for (int i = 0; i < Actors.Num(); i++) {
		UAnnotationComponent* Comp = Actors[i]->FindComponentByClass<UAnnotationComponent>();
		if (Comp) {
			AnnotationComps.Add(Comp);
		}		
	}
	UE_LOG(LogTemp, Error, TEXT("Found %d Annotation Components for this level"), AnnotationComps.Num());
}


void AAnnotationManager::SetupSegmentAnnotation() {
	int StencilValue = 1;
	TArray<AActor*> SegmentActors = FHelpers::FindActorsByTag("segment");
	if (SegmentActors.Num() > 0) {
		for (int i = 0; i < SegmentActors.Num(); i++) {
			FString ActorTag = SegmentActors[i]->Tags[0].ToString();
			TArray<UMeshComponent*> Components;			
			SegmentActors[i]->GetComponents<UMeshComponent>(Components);
			for (int32 j = 0; j < Components.Num(); j++) {
				UMeshComponent* MeshComponent = Components[j];				
				if (!MeshSegmentMap.Contains(ActorTag)) {
					MeshSegmentMap.Add(ActorTag, StencilValue++);
				}
				MeshComponent->SetRenderCustomDepth(true);
				MeshComponent->SetCustomDepthStencilValue(MeshSegmentMap[ActorTag]);		
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Found %d Actors with SEGMENT tag for this level"), SegmentActors.Num());
	} else {
		UE_LOG(LogTemp, Error, TEXT("No Actors with SEGMENT tag found!"));
	}
}

void AAnnotationManager::AnnotateNonBlocking(FString CaptureId, FString SavePath, uint8 Flag) {
	UE_LOG(LogTemp, Error, TEXT("Annotate Non-Blocking"));
	bBusy = true;
	FString FilePath = "";
	FString Data = "";
	int ActorIdx = 0;
	// Save Segmentation Annotation
	FilePath = SavePath + "/Annotations/Segment/" + CaptureId + ".csv";
	Data = "background,0\n";
	for (auto& Entry : MeshSegmentMap) {
		Data += Entry.Key + "," + FString::FromInt(Entry.Value) + "\n";
	}
	FFileHelper::SaveStringToFile(Data, *FilePath);
	// Save 3D OpenPose Annotation
	FilePath = SavePath + "/Annotations/OpenPose3D/" + CaptureId + ".csv";
	Data = "Id,Name,Nose,Neck,RShoulder,RElbow,RWrist,LShoulder,LElbow,LWrist,RHip,RKnee,RAnkle,LHip,LKnee,LAnkle,REye,LEye,REar,LEar\n";
	ActorIdx = 0;
	for (UAnnotationComponent* &Comp : AnnotationComps) {
		Comp->UpdatePoseAnnotation();
		FString Line = FString::FromInt(ActorIdx) + "," + Comp->ActorName;
		for (FVector& Landmark : Comp->OpenPoseLandmarks) {
			Line += ",\"[" + FString::SanitizeFloat(Landmark.X) + "," + FString::SanitizeFloat(Landmark.Y) + "," + FString::SanitizeFloat(Landmark.Z) + "]\"";
		}
		Data += Line + "\n";
	}
	FFileHelper::SaveStringToFile(Data, *FilePath);
	// Save 3D BoundingBox Annotation
	FilePath = SavePath + "/Annotations/BBox3D/" + CaptureId + ".csv";
	Data = "Id,Name,P_0,P_1,P_2,P_3,P_4,P_5,P_6,P_7\n";
	ActorIdx = 0;
	for (UAnnotationComponent* &Comp : AnnotationComps) {
		Comp->UpdateBBoxAnnotation();
		FString Line = FString::FromInt(ActorIdx) + "," + Comp->ActorName;
		for (FVector& Corner : Comp->BBoxCorners) {
			Line += ",\"[" + FString::SanitizeFloat(Corner.X) + "," + FString::SanitizeFloat(Corner.Y) + "," + FString::SanitizeFloat(Corner.Z) + "]\"";
		}
		Data += Line + "\n";
	}
	FFileHelper::SaveStringToFile(Data, *FilePath);
	bBusy = false;
}