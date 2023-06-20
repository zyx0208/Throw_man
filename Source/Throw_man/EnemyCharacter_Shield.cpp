// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter_Shield.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"	// 적의 현재 월드를 구하기 위해 사용
#include "Kismet/KismetMathLibrary.h"	// 캐릭터 회전하기 위해 사용
#include "Kismet/GameplayStatics.h"	// Player 캐릭터 구하기 위해 사용
#include "TimerManager.h"
#include "BulletProjectile.h"		// 라인 트레이스 생성
#include "BulletFunctionLibrary.h"
#include "HealthComponent.h"


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
	UnitDirection = Direction.GetSafeNormal();

	/* 공격 시작 조건 */
	// 조건1: 플레이어가 보이고, 거리가 300 이하
	if (bCanSeePlayer && Distance <= 300.f && !canAttack) {
		// 달리기
		canAttack = true;
		GetCharacterMovement()->MaxWalkSpeed = Speed * 2;
	}
	else if (canAttack && Distance <= 90.f) {
		UE_LOG(LogTemp, Log, TEXT("Push"));

		if (PlayerCharacter != nullptr) {
			UE_LOG(LogTemp, Log, TEXT("Player Hit!"));
			UHealthComponent* HealthComponent = PlayerCharacter->FindComponentByClass<UHealthComponent>();
			if (HealthComponent != nullptr) {
				HealthComponent->LoseHealth(Damage);
			}
		}

		// 공격 성공. 튕겨내기.
		PlayerCharacter->LaunchCharacter(Direction * Speed, false, true);

		// 속도 설정 초기화, 공격 가능 초기화
		GetCharacterMovement()->MaxWalkSpeed = Speed;
		canAttack = false;
	}
	else if ((canAttack && !bCanSeePlayer) || (canAttack && Distance > 1000)) {	// 평소 상태
		GetCharacterMovement()->MaxWalkSpeed = Speed;
		canAttack = false;
	}

	//UE_LOG(LogTemp, Log, TEXT("Distance: %f"), Distance);
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
		IgnoreActors) && Distance <= 1500) {
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