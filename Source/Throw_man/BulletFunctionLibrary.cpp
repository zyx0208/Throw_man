// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletFunctionLibrary.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"

bool UBulletFunctionLibrary::CanSeeActor(const UWorld* World,
											FVector Location,
											const AActor * TargetActor,
											TArray<const AActor*> IgnoreActors) {
	if (TargetActor == nullptr) {
		return false;
	}

	// ���� Ʈ���̽��� ��� ����
	FHitResult Hit;
	// ���� Ʈ���̽� ��������-�������
	FVector Start = Location;
	FVector End = TargetActor->GetActorLocation();

	// Ʈ���̽� ä��: �þ� �Ǵ��� ���� ��
	// ECC_Visibility: Visibility Ʈ���̽� ä��
	ECollisionChannel Channel = ECollisionChannel::ECC_GameTraceChannel2;

	// Enemy�� Player ���̿��� ���� �����ϴ� ��ü�� �ִ��� �Ǵ�
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(IgnoreActors);	// Player ����

	// ���� Ʈ���̽� ����
	World->LineTraceSingleByChannel(Hit, Start, End, Channel, QueryParams);

	// ���ӿ��� ���� Ʈ���̽� �����ֱ�
	DrawDebugLine(World, Start, End, FColor::Red);

	return !Hit.bBlockingHit;
}
