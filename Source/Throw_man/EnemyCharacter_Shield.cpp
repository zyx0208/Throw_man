// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter_Shield.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"	// ���� ���� ���带 ���ϱ� ���� ���
#include "Kismet/KismetMathLibrary.h"	// ĳ���� ȸ���ϱ� ���� ���
#include "Kismet/GameplayStatics.h"	// Player ĳ���� ���ϱ� ���� ���
#include "TimerManager.h"
#include "BulletProjectile.h"		// ���� Ʈ���̽� ����
#include "BulletFunctionLibrary.h"


// Sets default values
AEnemyCharacter_Shield::AEnemyCharacter_Shield()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// �þ� ����
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

	// ���� Player ĳ���� ���ϱ�
	ACharacter * PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	// Player ĳ���� �ٶ󺸱�
	bCanSeePlayer = LookAtActor(PlayerCharacter);

	/* ������ �غ�(�������� ���ϱ�) */
	// �÷��̾� ĳ���Ϳ��� �Ÿ�
	Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
	Distance = Direction.Size();

	/* ���� ���� Ÿ�̸� ���� ���� */
	// ����1: �÷��̾ ���̰�, �Ÿ��� 300 ����
	if (bCanSeePlayer) {
		//UE_LOG(LogTemp, Log, TEXT("I can see"));
	}
	if (bCanSeePlayer && Distance <= 300.f) {
		//UE_LOG(LogTemp, Log, TEXT("I'm in your range"));
		// ����2: �̹� Ÿ�̸Ӱ� �����Ǿ� ���� ���� ��
		if (!isAttackTimerSeted) {
			UE_LOG(LogTemp, Log, TEXT("Fire Timer Start"));
			// ���� ����
			GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyCharacter_Shield::Attack, AttackInterval, true, AttackDelay);
			isAttackTimerSeted = true;
		}
	}
		else if (isAttackTimerSeted) {	// �� �� �ƴϰ� �߻� Ÿ�̸� ���۵� ������ Ÿ�̸� ������ ��
			UE_LOG(LogTemp, Log, TEXT("Attack Timer Stop"));
			// �Ѿ� �߻� ����
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
		// ȸ�� ���
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
		// ȸ���� ����
		SetActorRotation(LookAtRotation);

		return true;
	}

	return false;
}

void AEnemyCharacter_Shield::Attack() {

	UE_LOG(LogTemp, Log, TEXT("Attack"));
}