// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InputComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "MyCharacter.generated.h"

UCLASS()
class CPPDEMO_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void MoveRight(float value);
	void MoveForward(float value);
	void LookRight(float value);
	void LookUp(float value);

	void SwapShoulderView();
	void SwitchPerspective();

	void CheckJump();
	
	UPROPERTY()
		bool jumping;
	UPROPERTY()
		bool bViewingOverRightShoulder;
	UPROPERTY()
		bool bThirdPersonPerspective;
	UPROPERTY()
		bool bSwitchingPerspective;

protected:
	UPROPERTY()
		UTimelineComponent* SwapShoulderViewTimeline;
	UPROPERTY()
		UCurveFloat* FloatCurve;
	UFUNCTION()
		void TimelineCallback(float val);
	UFUNCTION()
		void TimelineFinishedCallback();
	UPROPERTY()
		TEnumAsByte<ETimelineDirection::Type> TimelineDirection;

	void PlayTimeline();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
		UCameraComponent* cam;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
		USpringArmComponent* springarm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstPerson")
		USkeletalMesh* FirstPersonArmsMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstPerson")
		UClass* FirstPersonArmsAnimBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FirstPerson")
		USkeletalMeshComponent* FirstPersonArms;
};
