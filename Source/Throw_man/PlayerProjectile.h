// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerProjectile.generated.h"

UCLASS()
class THROW_MAN_API APlayerProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		class USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* damaged_sound;
private:
	UPROPERTY(VisibleAnywhere, Category = Projectile)
		class UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class APlayerProjectile> PlayerProjectile;

protected:
	UPROPERTY(EditAnywhere, Category = Particles)
		class UParticleSystem* HitParticles;

public:	
	// Sets default values for this actor's properties
	APlayerProjectile();
	UProjectileMovementComponent* getMovement();
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	

};
