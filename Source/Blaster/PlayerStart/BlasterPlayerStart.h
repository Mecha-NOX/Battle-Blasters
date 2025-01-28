// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "BlasterPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	ABlasterPlayerStart(const FObjectInitializer& ObjectInitializer);

private:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SpawnCheckArea;

public:
	FORCEINLINE USphereComponent* GetSpawnCheckArea() const { return SpawnCheckArea; }
};
