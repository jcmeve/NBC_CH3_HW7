// Fill out your copyright notice in the Description page of Project Settings.


#include "HW7_Player.h"

#include "EnhancedInputComponent.h"
#include "HW7_PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AHW7_Player::AHW7_Player()
	: Gravity(980),
	  bIsOnGround(true),
	  LookSpeed(90.0f),
	  Acceleration(2000),
	  DragConstant(0.0005f),
	  AirControl(0.7f),
	  GrountCheckTimer(0.0f)
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
	Bounds = SkeletalMeshComponent->Bounds.BoxExtent;
}

void AHW7_Player::Move(const FInputActionValue& Value)
{
	MoveInput = Value.Get<FVector>();
}

void AHW7_Player::Look(const FInputActionValue& Value)
{
	LookVector = Value.Get<FVector>();
}

void AHW7_Player::GroundProcess(float DeltaTime)
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	FCollisionShape Capsule = FCollisionShape::MakeCapsule(Bounds.X, Bounds.Z);
	GetWorld()->SweepSingleByChannel(Hit, GetActorLocation(),
	                                 GetActorLocation() + (FVector{0.0, 0.0, -1.0} * 5.0f
	                                 ), GetActorQuat(), ECC_Visibility, Capsule, CollisionParams
	);

	if (Hit.bBlockingHit)
	{
		GrountCheckTimer = 0.0f;
		if (!bIsOnGround)
		{
			FVector CurrentLocation = GetActorLocation();
			SetActorLocation(FVector{CurrentLocation.X, CurrentLocation.Y, Hit.ImpactPoint.Z + Bounds.Z});
		}
		bIsOnGround = true;
	}
	else
	{
		GrountCheckTimer += DeltaTime;
		if (GrountCheckTimer >= GrountCheckTime)
		{
			bIsOnGround = false;
		}
	}

	if (bIsOnGround)
	{
		FRotator Rotator = GetActorRotation();
		float NewRoll = FMath::FInterpTo(Rotator.Roll, 0.0, DeltaTime, 5.0f);
		float NewPitch = FMath::FInterpTo(Rotator.Pitch, 0.0, DeltaTime, 5.0f);
		SetActorRotation(FRotator{NewPitch, Rotator.Yaw, NewRoll});
	}
	else
	{
		FVector DragForce = -Velocity.GetSafeNormal() * (CurrentMoveSpeed * CurrentMoveSpeed * DragConstant);
		Velocity += DragForce * DeltaTime;
	}
}

void AHW7_Player::InputProcess(float DeltaTime)
{
	if (!MoveInput.IsNearlyZero())
	{
		if (bIsOnGround)
		{
			if (MoveInput.Z < 0.0f) //can't digging
			{
				MoveInput.Z = 0.0f;
			}
			else if (MoveInput.Z > 0.1f) //jump
			{
				Velocity.Z += (2 * DeltaTime * Acceleration);
			}
		}
		float _Acceleration = bIsOnGround ? Acceleration : Acceleration * AirControl;
		Velocity += MoveInput * DeltaTime * _Acceleration;
		MoveInput = FVector::ZeroVector;
	}
	else
	{
		if (bIsOnGround)
		{
			Velocity = FMath::VInterpTo(Velocity, FVector::ZeroVector, DeltaTime, 5.0f);
			CurrentMoveSpeed = Velocity.Length();
			if (CurrentMoveSpeed < 1.0f)
			{
				CurrentMoveSpeed = 0.0f;
				Velocity = FVector::ZeroVector;
			}
		}
	}
}

// Called every frame
void AHW7_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GroundProcess(DeltaTime);

	FVector DownVector = GetActorQuat().Inverse().RotateVector({0, 0, -1});
	Velocity += DownVector * Gravity * DeltaTime;

	InputProcess(DeltaTime);

	if (!Velocity.IsNearlyZero())
	{
		float DownSpeed = FVector::DotProduct(DownVector, Velocity);
		if (DownSpeed >= 0.0f)
		{
			if (bIsOnGround)
			{
				Velocity += -DownVector * DownSpeed;
			}
		}
		AddActorLocalOffset(Velocity * DeltaTime);
	}

	if (!LookVector.IsNearlyZero())
	{
		FRotator Rotator(bIsOnGround ? 0 : LookVector.Y, LookVector.X, bIsOnGround ? 0 : LookVector.Z);
		Rotator*=DeltaTime*LookSpeed;
		AddActorLocalRotation(Rotator.Quaternion());
		LookVector = FVector::ZeroVector;
		bIsTurning = true;
	}
	else
	{
		bIsTurning = false;
	}

	CurrentMoveSpeed = Velocity.Length();
	if (CurrentMoveSpeed < 1.0f)
	{
		CurrentMoveSpeed = 0.0f;
		Velocity = FVector::ZeroVector;
	}
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
