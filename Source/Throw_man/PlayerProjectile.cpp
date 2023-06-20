// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "MyCharacter.h"
#include "EnemyCharacter.h"
#include "EnemyCharacter_Shield.h"


// Sets default values
APlayerProjectile::APlayerProjectile()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComponent->InitSphereRadius(15.0f);
	SphereComponent->BodyInstance.SetCollisionProfileName(FName("PlayerProjectile"));
	SphereComponent->SetNotifyRigidBodyCollision(true);
	SphereComponent->OnComponentHit.AddDynamic(this, &APlayerProjectile::OnHit);


	RootComponent = SphereComponent;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = SphereComponent;
	ProjectileMovement->ProjectileGravityScale = 0.5f;
	ProjectileMovement->InitialSpeed = 0;
	ProjectileMovement->MaxSpeed = 0;
	ProjectileMovement->bRotationFollowsVelocity = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	
}



UProjectileMovementComponent* APlayerProjectile::getMovement()
{
	if (ProjectileMovement != nullptr) {
		return ProjectileMovement;
	}
	else {
		return nullptr;
	}
	
	
}

void APlayerProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	/* ������ �浹 ���� �Ǵ� */
	AEnemyCharacter* EnemyPistol = Cast<AEnemyCharacter>(OtherActor);
	AEnemyCharacter_Shield* EnemyShield = Cast<AEnemyCharacter_Shield>(OtherActor);
	if (EnemyPistol != nullptr || EnemyShield != nullptr) {
		UE_LOG(LogTemp, Log, TEXT("Enemy Hit!"));

		// ���⿡ ������ �� ����
		UE_LOG(LogTemp, Log, TEXT("Player Attack Success. Destroy Enemy."));
		
		// �� �ǰ� ���� ���
		UGameplayStatics::PlaySoundAtLocation(this, damaged_sound, GetActorLocation(), 1.0f);

		// ���� �� ����
		OtherActor->Destroy();
	}
	else {
		/* �ڷ���Ʈ ���� ���� �Ǵ� */
		PlayerCharacter->canTeleport = true;

		//�浹 ����Ʈ ���
		if (HitParticles != nullptr && ProjectileMovement->Velocity.Size() > 350.0f) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, GetActorLocation(), FRotator::ZeroRotator, (FVector)(FMath::RandRange(0.5f, 1.0f)));
		}


		//�浹 ȿ���� ���
		//���� ���Ⱑ 350f �̻��� ���� ������ ���� ���� ��� ( ���� Ƣ�涧 ��� ȿ���� �߻��ϴ°� ���� )
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Ball Hit Size : %f"), ProjectileMovement->Velocity.Size()));
		if (PlayerCharacter->Hit_shackles_sound != nullptr && ProjectileMovement->Velocity.Size() > 350.0f) {
			UGameplayStatics::PlaySoundAtLocation(this, PlayerCharacter->Hit_shackles_sound, GetActorLocation(), 1.5f, FMath::RandRange(0.7f, 1.3f));
		}
	}
}


