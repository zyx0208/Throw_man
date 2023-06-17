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

	// 라인 트레이스의 결과 저장
	FHitResult Hit;
	// 라인 트레이스 시작지점-대상지점
	FVector Start = Location;
	FVector End = TargetActor->GetActorLocation();

	// 트레이스 채널: 시야 판단을 위해 비교
	// ECC_Visibility: Visibility 트레이스 채널
	ECollisionChannel Channel = ECollisionChannel::ECC_GameTraceChannel2;

	// Enemy와 Player 사이에서 적을 방해하는 물체가 있는지 판단
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(IgnoreActors);	// Player 무시

	// 라인 트레이스 실행
	World->LineTraceSingleByChannel(Hit, Start, End, Channel, QueryParams);

	// 게임에서 라인 트레이스 보여주기
	DrawDebugLine(World, Start, End, FColor::Red);

	return !Hit.bBlockingHit;
}
