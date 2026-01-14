// Fill out your copyright notice in the Description page of Project Settings.


#include "HW7_GameMode.h"

#include "HW7_Player.h"
#include "HW7_PlayerController.h"

AHW7_GameMode::AHW7_GameMode()
{
	DefaultPawnClass = AHW7_Player::StaticClass();
	PlayerControllerClass = AHW7_PlayerController::StaticClass();
}
