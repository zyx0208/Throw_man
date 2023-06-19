// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter_Sword.h"

// Sets default values
AEnemyCharacter_Sword::AEnemyCharacter_Sword()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyCharacter_Sword::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCharacter_Sword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacter_Sword::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

