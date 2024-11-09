// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/SegmentCamera.h"


USegmentCamera::USegmentCamera(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	this->PrimaryComponentTick.bCanEverTick = true;
	ConstructorHelpers::FObjectFinder<UMaterial> PPM_SegColors(TEXT("Material'/Game/Annotation/Materials/PPM_SegStencil.PPM_SegStencil'"));
	this->SetPostProcessMaterial(PPM_SegColors.Object);
}