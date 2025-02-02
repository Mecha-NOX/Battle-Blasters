// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Components/Image.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());

	ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
	if (BlasterPlayerState)
	{
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
		if (BlasterCharacter)
		{
			BlasterCharacter->ServerSetPlayerName(BlasterPlayerState->GetPlayerName());
		}
	}
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
		HideDeathMessage();
		ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState)
		{
			BlasterCharacter->ServerSetPlayerName(BlasterPlayerState->GetPlayerName());
		}
	}
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay && 
		BlasterHUD->CharacterOverlay->HealthBar && 
		BlasterHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
}

void ABlasterPlayerController::UpdateDeathMessage(const FString KillerName)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->KilledBy &&
		BlasterHUD->CharacterOverlay->KillerName &&
		BlasterHUD->CharacterOverlay->OverlayImage;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->KillerName->SetText(FText::FromString(KillerName));
		BlasterHUD->CharacterOverlay->KillerName->SetVisibility(ESlateVisibility::Visible);
		BlasterHUD->CharacterOverlay->KilledBy->SetVisibility(ESlateVisibility::Visible);
		BlasterHUD->CharacterOverlay->OverlayImage->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABlasterPlayerController::HideDeathMessage()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->KilledBy &&
		BlasterHUD->CharacterOverlay->KillerName &&
		BlasterHUD->CharacterOverlay->OverlayImage;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->KillerName->SetText(FText::FromString(""));
		BlasterHUD->CharacterOverlay->KillerName->SetVisibility(ESlateVisibility::Collapsed);
		BlasterHUD->CharacterOverlay->KilledBy->SetVisibility(ESlateVisibility::Collapsed);
		BlasterHUD->CharacterOverlay->OverlayImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}
