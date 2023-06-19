// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "MyCharacter.h"
#include "kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//  Tick ��� �� �� (���� ��� ����)
	//PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Health <= 0 �̸� ���ӿ���
	if (Healths <= 0) {
		Healths = 0;

		UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
	}
}

void UHealthComponent::LoseHealth(int Amount) {
	//Healths = Healths - Amount; //�̹� ui �������Ʈ ������ -20���ְ� �־ �� �����ʿ� ����

	AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	PlayerCharacter->ChangeSubtractHealth();;
}

void UHealthComponent::SetHealth(int Value) {
	Healths = Value;
}

int UHealthComponent::GetHealth() {
	return Healths;
}