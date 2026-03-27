#include "BadCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PathManager.h"
#include "PathSegment.h"
#include "Engine/LocalPlayer.h"

ABadCharacter::ABadCharacter()
{
    // cube
    UStaticMeshComponent* Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
    Cube->SetupAttachment(RootComponent);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.StaticMesh"));
    if (CubeAsset.Succeeded())
    {
        Cube->SetStaticMesh(CubeAsset.Object);
        Cube->SetRelativeScale3D(FVector(0.5f, 0.5f, 1.0f));
    }

    // spring arm — no rotation from controller
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 600.f;
    CameraBoom->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->bInheritPitch = false;
    CameraBoom->bInheritYaw = false;
    CameraBoom->bInheritRoll = false;

    // camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // movement — world relative, no rotation
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.3f;
}

void ABadCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void ABadCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABadCharacter::Move);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
    }
}

void ABadCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    FVector MoveDir = FVector::ForwardVector * MovementVector.X
                    + FVector::RightVector * MovementVector.Y;

    if (PathManager)
    {
        APathSegment* Seg = PathManager->GetClosestSegment(GetActorLocation());

        if (Seg)
        {
            // if we switched segments start a blend
            if (Seg != LastSegment)
            {
                LastSegment = Seg;
                SegmentBlend = 0.f;
            }

            // blend up to 1 over 0.15 seconds
            SegmentBlend = FMath::Min(SegmentBlend + GetWorld()->GetDeltaSeconds() / 0.15f, 1.f);

            if (Seg->IsOutside(GetActorLocation()))
            {
                FVector OutDir = Seg->GetOutwardDirection(GetActorLocation());

                // smooth the outward direction using blend
                OutDir = OutDir * SegmentBlend;

                float OutwardAmount = FVector::DotProduct(MoveDir, OutDir);
                if (OutwardAmount > 0.f)
                {
                    MoveDir -= OutDir * OutwardAmount;
                }
            }
        }
    }

    AddMovementInput(FVector::ForwardVector, MoveDir.X);
    AddMovementInput(FVector::RightVector, MoveDir.Y);
}