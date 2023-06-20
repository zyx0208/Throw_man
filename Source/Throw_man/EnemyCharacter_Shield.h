// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter_Shield.generated.h"

UCLASS()
class THROW_MAN_API AEnemyCharacter_Shield : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LookAt, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SightSource;

public:
	// Sets default values for this character's properties
	AEnemyCharacter_Shield();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 전달된 액터 방향으로 Enemy 회전, 전달된 액터를 볼 수 있는지 확인
	bool LookAtActor(AActor* TargetActor);
	// 전달된 액터를 볼 수 있는지 확인
	//bool CanSeeActor(const AActor* TargetActor) const;

	// 적 캐릭터가 이번 프레임에 플레이어를 볼 수 있는가
	bool bCanSeePlayer = false;
	// 적 캐릭터가 이전 프레임에 플레이어를 볼 수 있는가
	bool bPreviousCanSeePlayer = false;

	// 적 캐릭터 설정 속도
	float Speed = 100.f;
	// 적 캐릭터 공격력
	int Damage = 20;

	// 적 이동
	FVector Direction;
	float Distance;
	FVector UnitDirection;

	void Attack();
	bool canAttack = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
