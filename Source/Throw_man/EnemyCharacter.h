// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class THROW_MAN_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LookAt, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SightSource;

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 전달된 액터 방향으로 Enemy 회전
	void LookAtActor(AActor* TargetActor);
	// 전달된 액터를 볼 수 있는지 확인
	bool CanSeeActor(const AActor* TargetActor) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
