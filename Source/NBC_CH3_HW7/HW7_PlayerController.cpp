// Fill out your copyright notice in the Description page of Project Settings.


#include "HW7_PlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"

AHW7_PlayerController::AHW7_PlayerController()
	: InputMappingContext(nullptr),
	  MoveAction(nullptr),
	  LookAction(nullptr)
{
	ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextFinder(TEXT("/Game/Inputs/IMC_Human.IMC_Human"));
	if (InputMappingContextFinder.Succeeded())
	{
		InputMappingContext = InputMappingContextFinder.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> LookActionFinder(TEXT("/Game/Inputs/LookAction.LookAction"));
	if (LookActionFinder.Succeeded())
	{
		LookAction = LookActionFinder.Object;
	}
	ConstructorHelpers::FObjectFinder<UInputAction> MoveActionFinder(TEXT("/Game/Inputs/MoveAction.MoveAction"));
	if (MoveActionFinder.Succeeded())
	{
		MoveAction = MoveActionFinder.Object;
	}
}

void AHW7_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	

	
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext,0);
			}
		}
	}
	
}
