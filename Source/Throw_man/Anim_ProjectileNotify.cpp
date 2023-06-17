// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim_ProjectileNotify.h"

void UAnim_ProjectileNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AMyCharacter* Player = Cast<AMyCharacter>(MeshComp->GetOwner());

	if (Player) {
		Player->SpawnProjectile();
	}
}