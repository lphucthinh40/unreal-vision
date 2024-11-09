// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/NormalCamera.h"


UNormalCamera::UNormalCamera(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	this->PrimaryComponentTick.bCanEverTick = true;
	ConstructorHelpers::FObjectFinder<UMaterial> PPM_Normal(TEXT("Material'/Game/Annotation/Materials/PPM_Normal.PPM_Normal'"));
	this->SetPostProcessMaterial(PPM_Normal.Object);
}