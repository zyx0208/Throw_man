// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	
	// Player 캐릭터와 Enemy 캐릭터의 거리
	//characterDistance = FVector::Distance(this->GetActorLocation(), PlayerCharacter->GetActorLocation());

	/* 움직일 준비(단위벡터 구하기) */
	// 플레이어 캐릭터와의 거리
	Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
	Distance = Direction.Size();
	//Direction = UKismetMathLibrary::GetDirectionUnitVector(this->GetActorLocation(), PlayerCharacter->GetActorLocation());
	//Direction = Direction.GetSafeNormal();	// 단위벡터
	//UE_LOG(LogTemp, Log, TEXT("Distance: %f"), Distance);
	// 플레이어가 보이고 있고, 플레이어와의 거리가 300이 넘을 때 움직이기
	/*
	if (bCanSeePlayer && Distance > 300.f) {
		//UE_LOG(LogTemp, Log, TEXT("Moving"));
		FVector Location = GetActorLocation();
		
		Location += UKismetMathLibrary::GetDirectionUnitVector(this->GetActorLocation(), PlayerCharacter->GetActorLocation()) * Speed * DeltaTime;
		UE_LOG(LogTemp, Log, TEXT("EnemyCurrentSpeed: %f"), FVector::DotProduct(GetVelocity(), GetActorRotation().Vector()));


		//UE_LOG(LogTemp, Log, TEXT("Location: %s"), Location.ToString());
		GetCharacterMovement()->Velocity = Location;
	}
	*/
	/* 발사 시작 타이머 설정 조건 */
	// 조건1: 플레이어가 보이고, 거리가 300 이하
	if (bCanSeePlayer) {
		//UE_LOG(LogTemp, Log, TEXT("I can see"));
	}
	if (bCanSeePlayer && Distance <= 300.f) {
		//UE_LOG(LogTemp, Log, TEXT("I'm in your range"));
		// 조건2: 이미 타이머가 설정되어 있지 않을 것
		if (!isFireTimerSeted) {
			UE_LOG(LogTemp, Log, TEXT("Fire Timer Start"));
			// 총알 발사 시작
			//FireBullet();
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyCharacter::FireBullet, FireInterval, true, FireDelay);
			isFireTimerSeted = true;
		}
	}
	else if (isFireTimerSeted) {	// 둘 다 아니고 발사 타이머 시작돼 있으면 타이머 중지할 것
		UE_LOG(LogTemp, Log, TEXT("Fire Timer Stop"));
		// 총알 발사 중지
		GetWorldTimerManager().ClearTimer(FireTimerHandle);
		isFireTimerSeted = false;
	}
	/*
	if (bCanSeePlayer) {
		UE_LOG(LogTemp, Log, TEXT("I can see"));
		if (Speed == 0.0f) {
			UE_LOG(LogTemp, Log, TEXT("Fire"));
			// 총알 발사 시작
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyCharacter::FireBullet, FireInterval, true, FireDelay);
		}
		else {
			// 총알 발사 중지
			GetWorldTimerManager().ClearTimer(FireTimerHandle);
		}
	}
	*/
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
	UE_LOG(LogTemp, Log, TEXT("Fire"));

	//효과음
	if (gun_sound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, gun_sound, GetActorLocation(), 1.0f);
	}

	//이펙트
	if (GunParticles != nullptr) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunParticles, GetMesh()->GetSocketLocation(FName("weapon_gunHall")), FRotator::ZeroRotator, (FVector)(0.2f));
	}

	FVector ForwardVector = GetActorForwardVector();
	float SpawnDistance = 40.f;
	FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("weapon_hand_r")) + (ForwardVector * SpawnDistance);
	// 새 총알 스폰하기
	GetWorld()->SpawnActor<ABulletProjectile>(BulletClass, SpawnLocation, GetActorRotation());
}
//weapon_hand_r