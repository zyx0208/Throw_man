// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Controller.h"
#include "PlayerProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "HealthComponent.h"

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
	isThrowing = false;
	canTeleport = false;
	Projectile = nullptr;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
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
	//���⶧���� �����ϰ� �����̱�
	GetCharacterMovement()->Velocity *= 0.8f;
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);

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
	}

}

void AMyCharacter::ThrowBall() {
	//��������
	if (!isThrowing)
	{
		isThrowing = true;

		ChargedTime = GetWorld()->GetTimeSeconds() - ChargedTime;
		ChargedTime = floor(ChargedTime * 100) / 100; //�Ҽ��� 2�ڸ� �Ʒ��δ� ����������
		ChargedTime = FMath::Clamp(ChargedTime, 0.0f, 1.0f); //0���� 1�� ���̷� ���� �α�(���Ѵ� ��¡ ����)



		//throw!!
		if (PlayerProjectile) {
			UE_LOG(LogTemp, Log, TEXT("Realesed :%f"), GetWorld()->GetTimeSeconds());

			UWorld* World = GetWorld();
			if (World) {
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;

				float SpawnDistance = 70.f;
				float Power = 1000.0f;
				FVector SpawnLocation = this->GetActorLocation() + (GetActorForwardVector() * SpawnDistance); //���� ����
				FRotator Rotation = GetActorForwardVector().Rotation(); //ȸ��

                Projectile = World->SpawnActor<APlayerProjectile>(PlayerProjectile, SpawnLocation, Rotation, SpawnParams);

				if (Projectile) {
					Projectile->SphereComponent->MoveIgnoreActors.Add(SpawnParams.Owner);
					FVector temp = UKismetMathLibrary::GetDirectionUnitVector(this->GetActorLocation(), SpawnLocation) * Power * ChargedTime;
					Projectile->getMovement()->Velocity = temp;

					//����Ÿ�� ���
					//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("ChargedTime : %f"), ChargedTime));
				}


			}
		}

		ChargedTime = 0.0f;
	}

	//�ڷ���Ʈ
	if (canTeleport)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Emerald, FString::Printf(TEXT("teleport")));
		FVector teleportPos = Projectile->GetActorLocation();
		this->SetActorRelativeLocation(teleportPos, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
		Projectile->Destroy();
		Projectile = nullptr;

		canTeleport = false;
		isThrowing = false;
	}
	
}