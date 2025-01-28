// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerStart.h"
#include "Components/SphereComponent.h"

ABlasterPlayerStart::ABlasterPlayerStart(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SpawnCheckArea = CreateDefaultSubobject<USphereComponent>(TEXT("SpawnCheckArea"));
	SpawnCheckArea->SetupAttachment(GetRootComponent());
	SpawnCheckArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SpawnCheckArea->SetSphereRadius(1500.f);
	SpawnCheckArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpawnCheckArea->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
}
