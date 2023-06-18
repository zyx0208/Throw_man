// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS(Blueprintable, BlueprintType)
class THROW_MAN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// �����ڷκ��� ü�� ������ ���´�
	void LoseHealth(int Amount);

	

	// �������� �ʱ� �� ���� ü�� ������ ��
	UPROPERTY(EditAnywhere, Category = Health, BlueprintReadWrite)
		int Healths = 100;

	UFUNCTION(BlueprintCallable)
		int GetHealth();

	UFUNCTION(BlueprintCallable)
		void SetHealth(int value);
};
