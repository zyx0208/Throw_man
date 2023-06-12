// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Engine/World.h"	// 적의 현재 월드를 구하기 위해 사용
#include "Kismet/KismetMathLibrary.h"	// 캐릭터 회전하기 위해 사용
#include "Kismet/GameplayStatics.h"	// Player 캐릭터 구하기 위해 사용
#include "TimerManager.h"
#include "BulletProjectile.h"
#include "BulletFunctionLibrary.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 시야 설정
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

	// 현재 Player 캐릭터 구하기
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	// Player 캐릭터 바라보기
	bCanSeePlayer = LookAtActor(PlayerCharacter);
	
	if (bCanSeePlayer != bPreviousCanSeePlayer) {
		if (bCanSeePlayer) {
			// 총알 발사 시작
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyCharacter::FireBullet, FireInterval, true, FireDelay);
		}
		else {
			// 총알 발사 중지
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
		// 회전 계산
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
		// 회전값 설정
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
	// 새 총알 스폰하기
	GetWorld()->SpawnActor<ABulletProjectile>(BulletClass, SpawnLocation, GetActorRotation());
}