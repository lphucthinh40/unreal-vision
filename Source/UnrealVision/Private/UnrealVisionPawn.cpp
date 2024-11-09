// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealVisionPawn.h"

#include "AnnotationManager.h"
#include "CaptureManager.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "UnrealVisionGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"

AUnrealVisionPawn::AUnrealVisionPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	this->SetHidden(true);
	this->SetTickableWhenPaused(true);
	WarmUpCounter = 0;
	WarmUpThreshold = 5; // wait for a few ticks before capturing data
}

void AUnrealVisionPawn::BeginPlay()
{
	OnDisableLookAround();
	GameMode = Cast<AUnrealVisionGameMode>(UGameplayStatics::GetGameMode(this));
	Super::BeginPlay();
}

void AUnrealVisionPawn::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );
	if (GameMode->bGeneratingDataset) {
		if (WarmUpCounter > WarmUpThreshold && !GameMode->CaptureManager->bBusy && !GameMode->AnnotationManager->bBusy) {
			(GameMode->CaptureManager)->CaptureNonBlocking(FString::FromInt(GameMode->CaptureIndex), GameMode->CurrentSavePath, 0xFF);
			(GameMode->AnnotationManager)->AnnotateNonBlocking(FString::FromInt(GameMode->CaptureIndex), GameMode->CurrentSavePath, 0xFF);
			ULevelSequencePlayer* Player = GameMode->LevelSequenceActor->GetSequencePlayer();
			UE_LOG(LogTemp, Error, TEXT("FRAME TIME: %d / %d"), Player->GetCurrentTime().Time.CeilToFrame().Value, Player->GetFrameDuration());
			if (Player->GetCurrentTime().Time.CeilToFrame() < Player->GetFrameDuration()) {
				FMovieSceneSequencePlaybackParams startParams = {};
				startParams.PositionType = EMovieScenePositionType::Frame;
				startParams.Frame = Player->GetCurrentTime().Time + 5; // 60/5=12fps
				startParams.UpdateMethod = EUpdatePositionMethod::Jump;
				Player->SetPlaybackPosition(startParams);
				GameMode->CaptureIndex += 1;
				GameMode->GeneratingDatasetProgress = 100 * FMath::Min(1.0, Player->GetCurrentTime().Time.CeilToFrame().Value / Player->GetFrameDuration());
			} else {
				GameMode->bGeneratingDataset = false;
				WarmUpCounter = 0;
			}
		} else {
			WarmUpCounter += 1;
		}
	} else	{
		WarmUpCounter = 0;
	}
}

void AUnrealVisionPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Setup Input Bindings
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::W, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::S, -1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::A, -1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::D, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("LookRight", EKeys::MouseX, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("LookUp", EKeys::MouseY, -1.f));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("EnableLookAround", EKeys::LeftShift));

	// Setup Input Handlers
	PlayerInputComponent->BindAxis("MoveForward", this, &AUnrealVisionPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUnrealVisionPawn::MoveRight);
	PlayerInputComponent->BindAxis("LookRight", this, &AUnrealVisionPawn::HandleLookRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AUnrealVisionPawn::HandleLookUp);
	PlayerInputComponent->BindAction("EnableLookAround", EInputEvent::IE_Pressed, this, &AUnrealVisionPawn::OnEnableLookAround);
	PlayerInputComponent->BindAction("EnableLookAround", EInputEvent::IE_Released, this, &AUnrealVisionPawn::OnDisableLookAround);
}

void AUnrealVisionPawn::OnEnableLookAround() {
	LookAroundEnabled = true;
	APlayerController* const PC = CastChecked<APlayerController>(Controller);
	PC->bShowMouseCursor = false;
	PC->bEnableClickEvents = false;
}

void AUnrealVisionPawn::OnDisableLookAround() {
	LookAroundEnabled = false;
	APlayerController* const PC = CastChecked<APlayerController>(Controller);
	PC->bShowMouseCursor = true;
	PC->bEnableClickEvents = true;
}

void AUnrealVisionPawn::HandleLookUp(float Val) {
	if (Val != 0.f && Controller && LookAroundEnabled) {
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddPitchInput(Val);
	}
}

void AUnrealVisionPawn::HandleLookRight(float Val) {
	if (Val != 0.f && Controller && LookAroundEnabled) {
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddYawInput(Val);
	}
}