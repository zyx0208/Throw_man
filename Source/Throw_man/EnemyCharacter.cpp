// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Engine/World.h"	// ���� ���� ���带 ���ϱ� ���� ���
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"	// ĳ���� ȸ���ϱ� ���� ���
#include "Kismet/GameplayStatics.h"	// Player ĳ���� ���ϱ� ���� ���

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

	// ���� Ʈ���̽��� ��� ����
	FHitResult Hit;
	// ���� Ʈ���̽� ��������-�������
	FVector Start = SightSource->GetComponentLocation();
	FVector End = TargetActor->GetActorLocation();

	// Ʈ���̽� ä��: �þ� �Ǵ��� ���� ��
	// ECC_Visibility: Visibility Ʈ���̽� ä��
	ECollisionChannel Channel = ECollisionChannel::ECC_GameTraceChannel2;

	// Enemy�� Player ���̿��� ���� �����ϴ� ��ü�� �ִ��� �Ǵ�
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);	// Enemy ����
	QueryParams.AddIgnoredActor(TargetActor);	// Player ����

	// ���� Ʈ���̽� ����
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, Channel, QueryParams);

	// ���ӿ��� ���� Ʈ���̽� �����ֱ�
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
		// ȸ�� ���
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
		// ȸ���� ����
		SetActorRotation(LookAtRotation);
	}
}
