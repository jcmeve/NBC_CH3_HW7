// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HW7_PlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
UCLASS()
class NBC_CH3_HW7_API AHW7_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AHW7_PlayerController();
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Input)
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Input)
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Input)
	UInputAction* LookAction;
	
};
