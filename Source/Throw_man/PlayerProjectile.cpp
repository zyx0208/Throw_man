// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "MyCharacter.h"


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
	PlayerCharacter->canTeleport = true;
	
	
	//충돌 효과음 재생
	//현재 족쇄가 350f 이상의 힘을 가지고 있을 때만 재생 ( 통통 튀길때 계속 효과음 발생하는거 방지 )
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Ball Hit Size : %f"), ProjectileMovement->Velocity.Size()));
	if (PlayerCharacter->Hit_shackles_sound != nullptr && ProjectileMovement->Velocity.Size() > 350.0f) {
		UGameplayStatics::PlaySoundAtLocation(this, PlayerCharacter->Hit_shackles_sound, GetActorLocation(), 1.5f, FMath::RandRange(0.7f, 1.3f));
	}

}


