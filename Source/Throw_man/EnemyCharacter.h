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
	
	// �� �̵�
	FVector Direction;
	float Distance;

	// �Ѿ� �߻� ���� Ÿ�̸�
	FTimerHandle FireTimerHandle;
	float FireInterval = 0.5f;
	float FireDelay = 0.1f;
	void FireBullet();
	bool isFireTimerSeted = false;

	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* gun_sound;

	UPROPERTY(EditAnywhere, Category = Particles)
		class UParticleSystem* GunParticles;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// bullet ������Ʈ�� �����ϴ� �� ����ϴ� Ŭ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bullet)
	TSubclassOf<class ABulletProjectile> BulletClass;

	
};
