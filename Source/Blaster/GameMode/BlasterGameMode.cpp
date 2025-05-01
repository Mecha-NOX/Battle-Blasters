// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/PlayerStart/BlasterPlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Components/SphereComponent.h"

namespace MatchState
{
	const FName Cooldown = FName(TEXT("Cooldown"));
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState);
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
		// Only update the death message if the game is in progress
		if (MatchState == MatchState::InProgress && AttackerPlayerState)
		{
			// Reset the death message to force RepNotify
			VictimPlayerState->UpdateDeathMessage(TEXT("")); // Clear first
			
			VictimPlayerState->UpdateDeathMessage(AttackerPlayerState->GetPlayerName());
		}
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		ABlasterPlayerStart* BestPlayerStart = SelectBestPlayerStart(ElimmedCharacter);

		// If no vacant spawn points, BestPlayerStart will be set in the SelectBestPlayerStart Function
		if (BestPlayerStart)
		{
			RestartPlayerAtPlayerStart(ElimmedController, BestPlayerStart);
		}
	}
}

ABlasterPlayerStart* ABlasterGameMode::SelectBestPlayerStart(ACharacter* ElimmedCharacter)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, ABlasterPlayerStart::StaticClass(), PlayerStarts);

	ABlasterPlayerStart* BestPlayerStart = nullptr;
	int32 LeastOverlapCount = INT_MAX; // Used to track the least number of overlaps
	float BestDistance = -1.f;

	TArray<ABlasterPlayerStart*> VacantSpawns; // To track spawns with no players

	for (AActor* StartActor : PlayerStarts)
	{
		ABlasterPlayerStart* PlayerStart = Cast<ABlasterPlayerStart>(StartActor);
		if (!PlayerStart) continue;

		// Get the spawn's SphereComponent
		USphereComponent* SpawnCheckArea = PlayerStart->GetSpawnCheckArea();
		if (!SpawnCheckArea) continue;

		// List of players within the spawn's sphere radius
		TArray<AActor*> OverlappingPlayers;
		SpawnCheckArea->GetOverlappingActors(OverlappingPlayers, ABlasterCharacter::StaticClass());

		int32 OverlapCount = OverlappingPlayers.Num();

		if (OverlapCount == 0) // Case 1: If no players overlap, mark as vacant spawn
		{
			VacantSpawns.Add(PlayerStart);
			continue;
		}
		if (OverlapCount < LeastOverlapCount) // Case 2: Track the least crowded spawn
		{
			LeastOverlapCount = OverlapCount;
			BestPlayerStart = PlayerStart;
			BestDistance = -1.f;  // Reset BestDistance as we have a better spawn candidate
		}
		else if (OverlapCount == LeastOverlapCount) // Case 3: Tie in overlap count, compare distance
		{
			// Find the distance to the closest player inside the radius for this spawn
			float ClosestDistance = FLT_MAX;
			for (AActor* OverlappingPlayer : OverlappingPlayers)
			{
				if (OverlappingPlayer && OverlappingPlayer != ElimmedCharacter)
				{
					float Distance = FVector::Dist(PlayerStart->GetActorLocation(), OverlappingPlayer->GetActorLocation());
					ClosestDistance = FMath::Min(ClosestDistance, Distance);
				}
			}
			// If this spawn has the same number of overlaps, but further players, choose this spawn
			// Update logic to choose the spawn with the farthest closest player
			if (BestPlayerStart == nullptr || ClosestDistance > BestDistance)
			{
				BestPlayerStart = PlayerStart;
				BestDistance = ClosestDistance;
			}
		}
	}

	// Case 4: If there are vacant spawns, select one at random
	if (VacantSpawns.Num() > 0)
	{
		BestPlayerStart = VacantSpawns[FMath::RandRange(0, VacantSpawns.Num() - 1)];
	}

	return BestPlayerStart;
}
