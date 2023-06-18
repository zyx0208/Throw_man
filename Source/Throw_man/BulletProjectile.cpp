// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletProjectile.h"
#include "Components/SphereComponent.h"
#include "MyCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "HealthComponent.h"

// Sets default values
ABulletProjectile::ABulletProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereComponent->SetSphereRadius(35.f);
	SphereComponent->SetCollisionProfileName(FName("Bullet"));
	SphereComponent->SetSimulatePhysics(true);

	// �ùķ��̼��� Hit �̺�Ʈ�� �߻���Ų��
	SphereComponent->SetNotifyRigidBodyCollision(true);

	// Root ������Ʈ ����
	RootComponent = SphereComponent;


	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	ProjectileMovement->InitialSpeed = 1500.f;
}	

// Called when the game starts or when spawned
void ABulletProjectile::BeginPlay()
{
	Super::BeginPlay();

	
	// �߰��� �Լ��� ���ε��� ���� OnComponentHit �̺�Ʈ�� �޵��� ����
	SphereComponent->OnComponentHit.AddDynamic(this, &ABulletProjectile::OnHit);
	
	/*
	// ��ü�� �浹 �߻� �� OnHit �Լ� ����
	FScriptDelegate ScriptDelegate;
	ScriptDelegate.BindUFunction(this, FName("OnHit"));
	SphereComponent->OnComponentHit.Add(ScriptDelegate);*/
	
	SetLifeSpan(5.f);
}

// Called every frame
void ABulletProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AMyCharacter* Player = Cast<AMyCharacter>(OtherActor);
	if (Player != nullptr) {
		UE_LOG(LogTemp, Log, TEXT("Player Hit!"));
		UHealthComponent* HealthComponent = Player->FindComponentByClass<UHealthComponent>();
		if (HealthComponent != nullptr) {
			HealthComponent->LoseHealth(Damage);
			Destroy();
		}
	}
	// ��ü�� ������ �Ѿ� ����
	UE_LOG(LogTemp, Log, TEXT("OnHit: Destroy"));
	Destroy();
}
