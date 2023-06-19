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
	
	// 적 이동
	FVector Direction;
	float Distance;

	// 총알 발사 간격 타이머
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

	// bullet 오브젝트를 생성하는 데 사용하는 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bullet)
	TSubclassOf<class ABulletProjectile> BulletClass;

	
};
