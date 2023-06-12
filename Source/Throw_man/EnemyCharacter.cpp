// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Engine/World.h"	// ���� ���� ���带 ���ϱ� ���� ���
#include "Kismet/KismetMathLibrary.h"	// ĳ���� ȸ���ϱ� ���� ���
#include "Kismet/GameplayStatics.h"	// Player ĳ���� ���ϱ� ���� ���
#include "TimerManager.h"
#include "BulletProjectile.h"
#include "BulletFunctionLibrary.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// �þ� ����
	SightSource = CreateDefaultSubobject<USceneComponent>(TEXT("Sight Source"));
	SightSource->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ���� Player ĳ���� ���ϱ�
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	// Player ĳ���� �ٶ󺸱�
	bCanSeePlayer = LookAtActor(PlayerCharacter);
	
	if (bCanSeePlayer != bPreviousCanSeePlayer) {
		if (bCanSeePlayer) {
			// �Ѿ� �߻� ����
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyCharacter::FireBullet, FireInterval, true, FireDelay);
		}
		else {
			// �Ѿ� �߻� ����
			GetWorldTimerManager().ClearTimer(FireTimerHandle);
		}
	}

	bPreviousCanSeePlayer = bCanSeePlayer;
	//UE_LOG(LogTemp, Log, TEXT("CanSeeActor :%d"), CanSeeActor(PlayerCharacter));
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AEnemyCharacter::LookAtActor(AActor* TargetActor) {
	if (TargetActor == nullptr) {
		return false;
	}

	const TArray<const AActor*> IgnoreActors = { this, TargetActor };

	if (UBulletFunctionLibrary::CanSeeActor(
		GetWorld(),
		SightSource->GetComponentLocation(),
		TargetActor,
		IgnoreActors)) {
		FVector Start = GetActorLocation();
		FVector End = TargetActor->GetActorLocation();
		// ȸ�� ���
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
		// ȸ���� ����
		SetActorRotation(LookAtRotation);

		return true;
	}

	return false;
}

void AEnemyCharacter::FireBullet() {
	if (BulletClass == nullptr) {
		return;
	}

	FVector ForwardVector = GetActorForwardVector();
	float SpawnDistance = 40.f;
	FVector SpawnLocation = GetActorLocation() + (ForwardVector * SpawnDistance);
	// �� �Ѿ� �����ϱ�
	GetWorld()->SpawnActor<ABulletProjectile>(BulletClass, SpawnLocation, GetActorRotation());
}