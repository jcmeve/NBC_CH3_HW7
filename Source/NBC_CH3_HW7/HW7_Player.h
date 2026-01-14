// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HW7_Player.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
struct FInputActionValue;

UCLASS()
class NBC_CH3_HW7_API AHW7_Player : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHW7_Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="HW7_Player")
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="HW7_Player|Camera")
	UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="HW7_Player|Camera")
	USpringArmComponent * SpringArmComponent;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="HW7_Player")
	USkeletalMeshComponent* SkeletalMeshComponent;
	
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="HW7_Player|Camera")
	double CameraLowerBound=60.0;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="HW7_Player|Camera")
	double CameraUpperBound=30.0;
	
	UPROPERTY(EditAnywhere,Category="HW7_Player|Movement")
	float MaxMoveSpeed;
	UPROPERTY(EditAnywhere,Category="HW7_Player|Movement")
	float Acceleration;
	
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="HW7_Player|Movement")
	float CurrentMoveSpeed;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="HW7_Player|Movement")
	bool bIsTurning;	
private:
	FVector MoveDir;
	FVector2D LookVector;
	FVector2D MoveInput;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
