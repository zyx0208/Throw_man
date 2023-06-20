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

	// ���޵� ���� �������� Enemy ȸ��, ���޵� ���͸� �� �� �ִ��� Ȯ��
	bool LookAtActor(AActor* TargetActor);
	// ���޵� ���͸� �� �� �ִ��� Ȯ��
	//bool CanSeeActor(const AActor* TargetActor) const;

	// �� ĳ���Ͱ� �̹� �����ӿ� �÷��̾ �� �� �ִ°�
	bool bCanSeePlayer = false;
	// �� ĳ���Ͱ� ���� �����ӿ� �÷��̾ �� �� �ִ°�
	bool bPreviousCanSeePlayer = false;

	// �� ĳ���� ���� �ӵ�
	float Speed = 100.f;
	// �� ĳ���� ���ݷ�
	int Damage = 20;

	// �� �̵�
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
