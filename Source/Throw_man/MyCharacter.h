// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Engine/SkeletalMesh.h"
#include "MyCharacter.generated.h"


UCLASS()   
class THROW_MAN_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MYCha_Cam, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MYCha_Cam, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class APlayerProjectile> PlayerProjectile;
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* ThrowMontage;
	
	class UHealthComponent* HealthComponent;
	/*
	UPROPERTY(EditAnywhere)
    USkeletalMeshComponent* ballMesh;
	*/


public:
	// Sets default values for this character's properties
	AMyCharacter();

	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* damaged_sound;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* throwing_shackles;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* Hit_shackles_sound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveRight(float Value);
	void MoveForward(float Value);

	//Throw Logic
	void PreThrowBall();
	void ThrowBall();
	float ChargedTime;
	class APlayerProjectile* Projectile;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnProjectile();

	UFUNCTION(BlueprintImplementableEvent)
		void setBallInVisible();
	UFUNCTION(BlueprintImplementableEvent)
		void setBallVisible();

	bool isCharging;
	bool isThrowing;
	bool canTeleport;


};
