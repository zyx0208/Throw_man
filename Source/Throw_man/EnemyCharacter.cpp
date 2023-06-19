// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	
	// Player ĳ���Ϳ� Enemy ĳ������ �Ÿ�
	//characterDistance = FVector::Distance(this->GetActorLocation(), PlayerCharacter->GetActorLocation());

	/* ������ �غ�(�������� ���ϱ�) */
	// �÷��̾� ĳ���Ϳ��� �Ÿ�
	Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
	Distance = Direction.Size();
	//Direction = UKismetMathLibrary::GetDirectionUnitVector(this->GetActorLocation(), PlayerCharacter->GetActorLocation());
	//Direction = Direction.GetSafeNormal();	// ��������
	//UE_LOG(LogTemp, Log, TEXT("Distance: %f"), Distance);
	// �÷��̾ ���̰� �ְ�, �÷��̾���� �Ÿ��� 300�� ���� �� �����̱�
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
	/* �߻� ���� Ÿ�̸� ���� ���� */
	// ����1: �÷��̾ ���̰�, �Ÿ��� 300 ����
	if (bCanSeePlayer) {
		//UE_LOG(LogTemp, Log, TEXT("I can see"));
	}
	if (bCanSeePlayer && Distance <= 300.f) {
		//UE_LOG(LogTemp, Log, TEXT("I'm in your range"));
		// ����2: �̹� Ÿ�̸Ӱ� �����Ǿ� ���� ���� ��
		if (!isFireTimerSeted) {
			UE_LOG(LogTemp, Log, TEXT("Fire Timer Start"));
			// �Ѿ� �߻� ����
			//FireBullet();
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyCharacter::FireBullet, FireInterval, true, FireDelay);
			isFireTimerSeted = true;
		}
	}
	else if (isFireTimerSeted) {	// �� �� �ƴϰ� �߻� Ÿ�̸� ���۵� ������ Ÿ�̸� ������ ��
		UE_LOG(LogTemp, Log, TEXT("Fire Timer Stop"));
		// �Ѿ� �߻� ����
		GetWorldTimerManager().ClearTimer(FireTimerHandle);
		isFireTimerSeted = false;
	}
	/*
	if (bCanSeePlayer) {
		UE_LOG(LogTemp, Log, TEXT("I can see"));
		if (Speed == 0.0f) {
			UE_LOG(LogTemp, Log, TEXT("Fire"));
			// �Ѿ� �߻� ����
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyCharacter::FireBullet, FireInterval, true, FireDelay);
		}
		else {
			// �Ѿ� �߻� ����
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
	UE_LOG(LogTemp, Log, TEXT("Fire"));

	//ȿ����
	if (gun_sound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, gun_sound, GetActorLocation(), 1.0f);
	}

	//����Ʈ
	if (GunParticles != nullptr) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunParticles, GetMesh()->GetSocketLocation(FName("weapon_gunHall")), FRotator::ZeroRotator, (FVector)(0.2f));
	}

	FVector ForwardVector = GetActorForwardVector();
	float SpawnDistance = 40.f;
	FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("weapon_hand_r")) + (ForwardVector * SpawnDistance);
	// �� �Ѿ� �����ϱ�
	GetWorld()->SpawnActor<ABulletProjectile>(BulletClass, SpawnLocation, GetActorRotation());
}
//weapon_hand_r