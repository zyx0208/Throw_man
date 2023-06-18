// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "PlayerProjectile.h"
#include "Components/SphereComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Throw logic
	ChargedTime = 0.0f;
	isCharging = false;
	isThrowing = false;
	canTeleport = false;
	Projectile = nullptr;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!canTeleport) {
		//���� �ް� ���� ���� �����ϰ� �����̱�
		GetCharacterMovement()->Velocity *= 0.8f;
	}

	//��¡�Ҷ��� ����ؼ� ��� ����
	if (isCharging) {
		//���� ���� ����
		float tempTime = ChargedTime;
		tempTime = GetWorld()->GetTimeSeconds() - tempTime;
		tempTime = floor(tempTime * 100) / 100; //�Ҽ��� 2�ڸ� �Ʒ��δ� ����������
		tempTime = FMath::Clamp(tempTime, 0.0f, 1.0f); //0���� 1�� ���̷� ���� �α�(���Ѵ� ��¡ ����)

		float SpawnDistance = 70.f;
		float Power = 1000.0f;

		FVector SpawnLocation = this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")) + (GetActorForwardVector() * SpawnDistance); //���Ͽ��� ������
		FVector outVelocity = UKismetMathLibrary::GetDirectionUnitVector(this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")), SpawnLocation) * Power * tempTime;
		FPredictProjectilePathParams predictParams(10.0f, SpawnLocation, outVelocity, 1.0f);   // 20: tracing ������ ������Ÿ�� ũ��, 15: �ù����̼ǵǴ� Max �ð�(��)
		predictParams.DrawDebugType = EDrawDebugTrace::Type::ForOneFrame;  // DrawDebugTime �� �����ϸ� EDrawDebugTrace::Type::ForDuration �ʿ�.
		predictParams.OverrideGravityZ = 0;
		FPredictProjectilePathResult result;
		UGameplayStatics::PredictProjectilePath(this, predictParams, result);

		/*�� ������ ������ ����. �Ʒ� �������� �ؾ� ��Ȯ�ѵ� �ٸ��� �� ���ϴϱ� ���� �ϰ� �ϰ���..
		FVector startLoc = this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")) + (GetActorForwardVector() * SpawnDistance);
		FVector targetLoc = UKismetMathLibrary::GetDirectionUnitVector(this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")), startLoc) * Power * tempTime;
		float arcValue = 0.5f;
		FVector outVelocity = FVector::ZeroVector;

		if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, startLoc, targetLoc, arcValue)) {
			FPredictProjectilePathParams predictParams(10.0f, startLoc, outVelocity, 1.0f);   // 20: tracing ������ ������Ÿ�� ũ��, 15: �ù����̼ǵǴ� Max �ð�(��)
			predictParams.DrawDebugType = EDrawDebugTrace::Type::ForOneFrame;  // DrawDebugTime �� �����ϸ� EDrawDebugTrace::Type::ForDuration �ʿ�.
			predictParams.OverrideGravityZ = 0;
			FPredictProjectilePathResult result;
			UGameplayStatics::PredictProjectilePath(this, predictParams, result);
		}


		/*
		if (UGameplayStatics::SuggestProjectileVelocity(this, outVelocity, startLoc, targetLoc, Power * tempTime)) {
				FPredictProjectilePathParams predictParams(10.0f, startLoc, outVelocity, 1.0f);   // 20: tracing ������ ������Ÿ�� ũ��, 15: �ù����̼ǵǴ� Max �ð�(��)
				predictParams.DrawDebugType = EDrawDebugTrace::Type::ForOneFrame;  // DrawDebugTime �� �����ϸ� EDrawDebugTrace::Type::ForDuration �ʿ�.
				predictParams.OverrideGravityZ = 0;
				FPredictProjectilePathResult result;
				UGameplayStatics::PredictProjectilePath(this, predictParams, result);
		}*/
	}

	//���⸦ ���� ����(== �ڷ���Ʈ ������ ����) �̸� ĳ������ �̵� ���� ����
	if (canTeleport) {
		if (Projectile) {

			//��Ÿ��� ������ �������� ĳ���Ϳ� ������ �Ÿ� ���ϱ�
			float Distance = FVector::Distance(this->GetActorLocation(), Projectile->GetActorLocation());
			//UE_LOG(LogTemp, Log, TEXT("Distance :%f"), Distance);


			if (Distance > 700.f) {
				//�÷��̾��� ��ġ�� ��������� �Ÿ��� 700�� �Ǵ� ���� ������ x,y������ �̵��� �����ϴ�.
				FVector temp = Projectile->GetActorLocation();
				FVector MAXVector = this->GetActorLocation() + UKismetMathLibrary::GetDirectionUnitVector(this->GetActorLocation(), temp) * 700;

				//���� �̵��Ѵٸ� ���� ��ġ�� �ڷ���Ʈ ���� �̵� �Ÿ��� �����Ѵ�
				SetActorRelativeLocation(MAXVector, false, (FHitResult*)nullptr, ETeleportType::ResetPhysics);
			}


		}

	}

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//Throw
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AMyCharacter::PreThrowBall);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &AMyCharacter::ThrowBall);

	
}

void AMyCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector Direction = UKismetMathLibrary::GetRightVector(YawRotation);
		AddMovementInput(Direction, Value);
		
		
	}
}

void AMyCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector Direction = UKismetMathLibrary::GetForwardVector(YawRotation);
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::PreThrowBall() {

	if (!isThrowing)
	{
		UE_LOG(LogTemp, Log, TEXT("Pressed :%f"), GetWorld()->GetTimeSeconds());
		ChargedTime = GetWorld()->GetTimeSeconds();
	
		isCharging = true;

	  
	}

}

void AMyCharacter::ThrowBall() {
	//��������
	if (!isThrowing)
	{
		isCharging = false;
		isThrowing = true;

		ChargedTime = GetWorld()->GetTimeSeconds() - ChargedTime;
		ChargedTime = floor(ChargedTime * 100) / 100; //�Ҽ��� 2�ڸ� �Ʒ��δ� ����������
		ChargedTime = FMath::Clamp(ChargedTime, 0.0f, 1.0f); //0���� 1�� ���̷� ���� �α�(���Ѵ� ��¡ ����)


		//throw!!
		if (PlayerProjectile&&ThrowMontage) {
			UE_LOG(LogTemp, Log, TEXT("Realesed :%f"), GetWorld()->GetTimeSeconds());

			bool isMontagePlaying = GetMesh()->GetAnimInstance()->Montage_IsPlaying(ThrowMontage);
			if (!isMontagePlaying) {
				GetMesh()->GetAnimInstance()->Montage_Play(ThrowMontage, 1.0f);
			}

			
		}

		
	}

	//�ڷ���Ʈ
	if (canTeleport)
	{
		if (Projectile)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Emerald, FString::Printf(TEXT("teleport")));
			FVector teleportPos = Projectile->GetActorLocation();
			teleportPos.Z += 100.0f;
			this->SetActorRelativeLocation(teleportPos, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
			Projectile->Destroy();
			Projectile = nullptr;

			canTeleport = false;
			isThrowing = false;

			setBallVisible();
		}
		
	}

}

//UAnim_ProjectileNotify::Notify()  
void AMyCharacter::SpawnProjectile()
{
	UWorld* World = GetWorld();
	if (World) {
		
		//�� �Ⱥ��̰� �ϱ�
		setBallInVisible();

		//������
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		float SpawnDistance = 70.f;
		float Power = 1000.0f;
		//FVector SpawnLocation = this->GetActorLocation() + (GetActorForwardVector() * SpawnDistance); //���� ����
		FVector SpawnLocation = this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")) + (GetActorForwardVector() * SpawnDistance); //���Ͽ��� ������
		FRotator Rotation = GetActorForwardVector().Rotation(); //ȸ��

		Projectile = World->SpawnActor<APlayerProjectile>(PlayerProjectile, SpawnLocation, Rotation, SpawnParams);

		if (Projectile) {
			Projectile->SphereComponent->MoveIgnoreActors.Add(SpawnParams.Owner);
			FVector temp = UKismetMathLibrary::GetDirectionUnitVector(this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")), SpawnLocation) * Power * ChargedTime;
			Projectile->getMovement()->Velocity = temp;

			//����Ÿ�� ���
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("ChargedTime : %f"), ChargedTime));
		}


	}

	ChargedTime = 0.0f;
}