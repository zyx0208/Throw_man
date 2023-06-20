// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter_Shield.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"	// ���� ���� ���带 ���ϱ� ���� ���
#include "Kismet/KismetMathLibrary.h"	// ĳ���� ȸ���ϱ� ���� ���
#include "Kismet/GameplayStatics.h"	// Player ĳ���� ���ϱ� ���� ���
#include "TimerManager.h"
#include "BulletProjectile.h"		// ���� Ʈ���̽� ����
#include "BulletFunctionLibrary.h"
#include "HealthComponent.h"


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
	UnitDirection = Direction.GetSafeNormal();

	/* ���� ���� ���� */
	// ����1: �÷��̾ ���̰�, �Ÿ��� 300 ����
	if (bCanSeePlayer && Distance <= 300.f && !canAttack) {
		// �޸���
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

		// ���� ����. ƨ�ܳ���.
		PlayerCharacter->LaunchCharacter(Direction * Speed, false, true);

		// �ӵ� ���� �ʱ�ȭ, ���� ���� �ʱ�ȭ
		GetCharacterMovement()->MaxWalkSpeed = Speed;
		canAttack = false;
	}
	else if ((canAttack && !bCanSeePlayer) || (canAttack && Distance > 1000)) {	// ��� ����
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