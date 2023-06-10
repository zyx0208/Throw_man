// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Engine/World.h"	// 적의 현재 월드를 구하기 위해 사용
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"	// 캐릭터 회전하기 위해 사용
#include "Kismet/GameplayStatics.h"	// Player 캐릭터 구하기 위해 사용

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
	LookAtActor(PlayerCharacter);
	//UE_LOG(LogTemp, Log, TEXT("CanSeeActor :%d"), CanSeeActor(PlayerCharacter));
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AEnemyCharacter::CanSeeActor(const AActor* TargetActor) const {
	if (TargetActor == nullptr) {
		return false;
	}

	// 라인 트레이스의 결과 저장
	FHitResult Hit;
	// 라인 트레이스 시작지점-대상지점
	FVector Start = SightSource->GetComponentLocation();
	FVector End = TargetActor->GetActorLocation();

	// 트레이스 채널: 시야 판단을 위해 비교
	// ECC_Visibility: Visibility 트레이스 채널
	ECollisionChannel Channel = ECollisionChannel::ECC_GameTraceChannel2;

	// Enemy와 Player 사이에서 적을 방해하는 물체가 있는지 판단
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);	// Enemy 무시
	QueryParams.AddIgnoredActor(TargetActor);	// Player 무시

	// 라인 트레이스 실행
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, Channel, QueryParams);

	// 게임에서 라인 트레이스 보여주기
	DrawDebugLine(GetWorld(), Start, End, FColor::Red);

	return !Hit.bBlockingHit;
}

void AEnemyCharacter::LookAtActor(AActor* TargetActor) {
	if (TargetActor == nullptr) {
		return;
	}

	if (CanSeeActor(TargetActor)) {
		FVector Start = GetActorLocation();
		FVector End = TargetActor->GetActorLocation();
		// 회전 계산
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
		// 회전값 설정
		SetActorRotation(LookAtRotation);
	}
}
