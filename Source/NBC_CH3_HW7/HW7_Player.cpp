// Fill out your copyright notice in the Description page of Project Settings.


#include "HW7_Player.h"
#include "EnhancedInputComponent.h"
#include "HW7_PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AHW7_Player::AHW7_Player()
	: MaxMoveSpeed(600.0f),
	  Acceleration(1000.0f)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneRoot = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(SceneRoot);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(SceneRoot);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	SkeletalMeshComponent->SetupAttachment(SceneRoot);
	SkeletalMeshComponent->AddLocalOffset({0, 0, -90.0});
	SkeletalMeshComponent->SetRelativeRotation({0.0, -90.0, 0.0});

	SkeletalMeshComponent->SetSimulatePhysics(false);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshFinder(
		TEXT("/Game/IdaFaber/Meshes/Girl/SK_MechanicGirl_01.SK_MechanicGirl_01"));
	if (SkeletalMeshFinder.Succeeded())
	{
		SkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshFinder.Object);
	}
	ConstructorHelpers::FClassFinder<UAnimInstance> AnimBlueprintFinder(
		TEXT("/Game/Animations/BP_HW7_Anim.BP_HW7_Anim_C"));
	if (AnimBlueprintFinder.Succeeded())
	{
		SkeletalMeshComponent->SetAnimInstanceClass(AnimBlueprintFinder.Class);
	}
}

// Called when the game starts or when spawned
void AHW7_Player::BeginPlay()
{
	Super::BeginPlay();
}

void AHW7_Player::Move(const FInputActionValue& Value)
{
	MoveInput = Value.Get<FVector2D>();
	MoveDir =  {MoveInput.X , MoveInput.Y,0.0};
	MoveDir.Normalize();
}

void AHW7_Player::Look(const FInputActionValue& Value)
{
	LookVector = Value.Get<FVector2D>();
}

// Called every frame
void AHW7_Player::Tick(float DeltaTime)
{
	if (!MoveInput.IsNearlyZero())
	{
		CurrentMoveSpeed+=Acceleration*DeltaTime;
		CurrentMoveSpeed = CurrentMoveSpeed >MaxMoveSpeed ? MaxMoveSpeed:CurrentMoveSpeed;
		MoveInput = FVector2D::ZeroVector;
	}	
	else
	{
		CurrentMoveSpeed = FMath::Lerp(CurrentMoveSpeed,0.0f,0.02f);
		if (CurrentMoveSpeed <0.1f)
		{
			CurrentMoveSpeed = 0.0f;
			MoveDir = FVector::ZeroVector;
			
		}
	}
	
	if (!FMath::IsNearlyZero(CurrentMoveSpeed))
	{
		AddActorLocalOffset(MoveDir * CurrentMoveSpeed * DeltaTime);
	}

	if (!LookVector.IsNearlyZero())
	{
		AddActorLocalRotation(FRotator(0, LookVector.X, 0.0f));
		double TargetPitch = SpringArmComponent->GetRelativeRotation().Pitch - LookVector.Y;
		TargetPitch = FMath::Clamp(TargetPitch, -CameraLowerBound, CameraUpperBound);
		SpringArmComponent->SetRelativeRotation(FRotator(TargetPitch, 0, 0.0f));
		LookVector = FVector2D::ZeroVector;
		bIsTurning = true;
	}
	else
	{
		bIsTurning = false;
	}

	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AHW7_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (const AHW7_PlayerController* PlayerController = Cast<AHW7_PlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this,
				                                   &AHW7_Player::Move);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInputComponent->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this,
				                                   &AHW7_Player::Look);
			}
		}
	}
}
