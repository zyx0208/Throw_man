// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter_Shield.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"	// 적의 현재 월드를 구하기 위해 사용
#include "Kismet/KismetMathLibrary.h"	// 캐릭터 회전하기 위해 사용
#include "Kismet/GameplayStatics.h"	// Player 캐릭터 구하기 위해 사용
#include "TimerManager.h"
#include "BulletProjectile.h"		// 라인 트레이스 생성
#include "BulletFunctionLibrary.h"


// Sets default values
AEnemyCharacter_Shield::AEnemyCharacter_Shield()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 시야 설정
	SightSource = CreateDefaultSubobject<USceneComponent>(TEXT("Sight Source"));
	SightSource->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyCharacter_Shield::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCharacter_Shield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 Player 캐릭터 구하기
	ACharacter * PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	// Player 캐릭터 바라보기
	bCanSeePlayer = LookAtActor(PlayerCharacter);

	/* 움직일 준비(단위벡터 구하기) */
	// 플레이어 캐릭터와의 거리
	Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
	Distance = Direction.Size();

	/* 공격 시작 타이머 설정 조건 */
	// 조건1: 플레이어가 보이고, 거리가 300 이하
	if (bCanSeePlayer) {
		//UE_LOG(LogTemp, Log, TEXT("I can see"));
	}
	if (bCanSeePlayer && Distance <= 300.f) {
		//UE_LOG(LogTemp, Log, TEXT("I'm in your range"));
		// 조건2: 이미 타이머가 설정되어 있지 않을 것
		if (!isAttackTimerSeted) {
			UE_LOG(LogTemp, Log, TEXT("Fire Timer Start"));
			// 공격 시작
			GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyCharacter_Shield::Attack, AttackInterval, true, AttackDelay);
			isAttackTimerSeted = true;
		}
	}
		else if (isAttackTimerSeted) {	// 둘 다 아니고 발사 타이머 시작돼 있으면 타이머 중지할 것
			UE_LOG(LogTemp, Log, TEXT("Attack Timer Stop"));
			// 총알 발사 중지
			GetWorldTimerManager().ClearTimer(AttackTimerHandle);
			isAttackTimerSeted = false;
		}

		bPreviousCanSeePlayer = bCanSeePlayer;
}

// Called to bind functionality to input
void AEnemyCharacter_Shield::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AEnemyCharacter_Shield::LookAtActor(AActor* TargetActor) {
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

void AEnemyCharacter_Shield::Attack() {

	UE_LOG(LogTemp, Log, TEXT("Attack"));
}