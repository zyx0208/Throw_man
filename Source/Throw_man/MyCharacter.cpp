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
		//족쇄 달고 있을 때는 느릿하게 움직이기
		GetCharacterMovement()->Velocity *= 0.8f;
	}

	//차징할때는 계속해서 경로 예측
	if (isCharging) {
		//던질 궤적 예측
		float tempTime = ChargedTime;
		tempTime = GetWorld()->GetTimeSeconds() - tempTime;
		tempTime = floor(tempTime * 100) / 100; //소수점 2자리 아래로는 버려버리기
		tempTime = FMath::Clamp(tempTime, 0.0f, 1.0f); //0에서 1초 사이로 차지 두기(무한대 차징 금지)

		float SpawnDistance = 70.f;
		float Power = 1000.0f;

		FVector SpawnLocation = this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")) + (GetActorForwardVector() * SpawnDistance); //소켓에서 던지기
		FVector outVelocity = UKismetMathLibrary::GetDirectionUnitVector(this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")), SpawnLocation) * Power * tempTime;
		FPredictProjectilePathParams predictParams(10.0f, SpawnLocation, outVelocity, 1.0f);   // 20: tracing 보여질 프로젝타일 크기, 15: 시물레이션되는 Max 시간(초)
		predictParams.DrawDebugType = EDrawDebugTrace::Type::ForOneFrame;  // DrawDebugTime 을 지정하면 EDrawDebugTrace::Type::ForDuration 필요.
		predictParams.OverrideGravityZ = 0;
		FPredictProjectilePathResult result;
		UGameplayStatics::PredictProjectilePath(this, predictParams, result);

		/*위 로직은 오차가 있음. 아래 로직으로 해야 정확한데 다른게 더 급하니까 딴거 하고 하겠음..
		FVector startLoc = this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")) + (GetActorForwardVector() * SpawnDistance);
		FVector targetLoc = UKismetMathLibrary::GetDirectionUnitVector(this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")), startLoc) * Power * tempTime;
		float arcValue = 0.5f;
		FVector outVelocity = FVector::ZeroVector;

		if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, startLoc, targetLoc, arcValue)) {
			FPredictProjectilePathParams predictParams(10.0f, startLoc, outVelocity, 1.0f);   // 20: tracing 보여질 프로젝타일 크기, 15: 시물레이션되는 Max 시간(초)
			predictParams.DrawDebugType = EDrawDebugTrace::Type::ForOneFrame;  // DrawDebugTime 을 지정하면 EDrawDebugTrace::Type::ForDuration 필요.
			predictParams.OverrideGravityZ = 0;
			FPredictProjectilePathResult result;
			UGameplayStatics::PredictProjectilePath(this, predictParams, result);
		}


		/*
		if (UGameplayStatics::SuggestProjectileVelocity(this, outVelocity, startLoc, targetLoc, Power * tempTime)) {
				FPredictProjectilePathParams predictParams(10.0f, startLoc, outVelocity, 1.0f);   // 20: tracing 보여질 프로젝타일 크기, 15: 시물레이션되는 Max 시간(초)
				predictParams.DrawDebugType = EDrawDebugTrace::Type::ForOneFrame;  // DrawDebugTime 을 지정하면 EDrawDebugTrace::Type::ForDuration 필요.
				predictParams.OverrideGravityZ = 0;
				FPredictProjectilePathResult result;
				UGameplayStatics::PredictProjectilePath(this, predictParams, result);
		}*/
	}

	//족쇄를 던진 상태(== 텔레포트 가능한 상태) 이면 캐릭터의 이동 범위 제한
	if (canTeleport) {
		if (Projectile) {

			//피타고라스 정리를 바탕으로 캐릭터와 족쇄의 거리 구하기
			float Distance = FVector::Distance(this->GetActorLocation(), Projectile->GetActorLocation());
			//UE_LOG(LogTemp, Log, TEXT("Distance :%f"), Distance);


			if (Distance > 700.f) {
				//플레이어의 위치는 족쇄까지의 거리가 700이 되는 원의 방정식 x,y에서만 이동이 가능하다.
				FVector temp = Projectile->GetActorLocation();
				FVector MAXVector = this->GetActorLocation() + UKismetMathLibrary::GetDirectionUnitVector(this->GetActorLocation(), temp) * 700;

				//만약 이동한다면 족쇄 위치로 텔레포트 시켜 이동 거리를 제한한다
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
	//공던지기
	if (!isThrowing)
	{
		isCharging = false;
		isThrowing = true;

		ChargedTime = GetWorld()->GetTimeSeconds() - ChargedTime;
		ChargedTime = floor(ChargedTime * 100) / 100; //소수점 2자리 아래로는 버려버리기
		ChargedTime = FMath::Clamp(ChargedTime, 0.0f, 1.0f); //0에서 1초 사이로 차지 두기(무한대 차징 금지)


		//throw!!
		if (PlayerProjectile&&ThrowMontage) {
			UE_LOG(LogTemp, Log, TEXT("Realesed :%f"), GetWorld()->GetTimeSeconds());

			bool isMontagePlaying = GetMesh()->GetAnimInstance()->Montage_IsPlaying(ThrowMontage);
			if (!isMontagePlaying) {
				GetMesh()->GetAnimInstance()->Montage_Play(ThrowMontage, 1.0f);
			}

			
		}

		
	}

	//텔레포트
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
		
		//공 안보이게 하기
		setBallInVisible();

		//던지기
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		float SpawnDistance = 70.f;
		float Power = 1000.0f;
		//FVector SpawnLocation = this->GetActorLocation() + (GetActorForwardVector() * SpawnDistance); //던질 방향
		FVector SpawnLocation = this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")) + (GetActorForwardVector() * SpawnDistance); //소켓에서 던지기
		FRotator Rotation = GetActorForwardVector().Rotation(); //회전

		Projectile = World->SpawnActor<APlayerProjectile>(PlayerProjectile, SpawnLocation, Rotation, SpawnParams);

		if (Projectile) {
			Projectile->SphereComponent->MoveIgnoreActors.Add(SpawnParams.Owner);
			FVector temp = UKismetMathLibrary::GetDirectionUnitVector(this->GetMesh()->GetSocketLocation(FName("ProjectileSocket")), SpawnLocation) * Power * ChargedTime;
			Projectile->getMovement()->Velocity = temp;

			//차지타임 출력
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("ChargedTime : %f"), ChargedTime));
		}


	}

	ChargedTime = 0.0f;
}