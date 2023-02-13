#include "MyCharacter.h"

AMyCharacter::AMyCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	UCharacterMovementComponent* const MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
	}

	jumping = false;
	bViewingOverRightShoulder = true;
	bThirdPersonPerspective = true;
	bSwitchingPerspective = false;

	cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	springarm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	FirstPersonArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonArms"));

	springarm->SetupAttachment(RootComponent);
	springarm->TargetArmLength = 200.0f;
	springarm->SetRelativeLocation(FVector(0.f, 0.f, 75.0f));
	springarm->SocketOffset = FVector(0.f, 75.0f, 0.f);
	springarm->bUsePawnControlRotation = true;

	cam->SetupAttachment(springarm, USpringArmComponent::SocketName);

	auto Curve = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("Curve'/Game/Curves/C_SwapShoulderView'"));
	check(Curve.Succeeded());
	FloatCurve = Curve.Object;

	FirstPersonArms->SetHiddenInGame(true);
	FirstPersonArms->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay() {
	
	FOnTimelineFloat onTimelineCallback;
	FOnTimelineEventStatic onTimelineFinishedCallback;

	Super::BeginPlay();

	if (FloatCurve != NULL)	{

		SwapShoulderViewTimeline = NewObject<UTimelineComponent>(this, FName("TimelineAnimation"));
		UTimelineComponent* TL = SwapShoulderViewTimeline;

		this->BlueprintCreatedComponents.Add(TL); // Add to array so it gets saved

		TL->SetPropertySetObject(this); // Set which object the timeline should drive properties on
		TL->SetDirectionPropertyName(FName("TimelineDirection"));

		TL->SetLooping(false);
		TL->SetTimelineLength(0.25f);
		TL->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

		TL->SetPlaybackPosition(0.0f, false);

		onTimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
		onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
		TL->AddInterpFloat(FloatCurve, onTimelineCallback);
		TL->SetTimelineFinishedFunc(onTimelineFinishedCallback);

		TL->RegisterComponent();
	}
	
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (jumping) Jump();

	if (SwapShoulderViewTimeline != NULL)
	{
		SwapShoulderViewTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	InputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	InputComponent->BindAxis("LookRight", this, &AMyCharacter::LookRight);
	InputComponent->BindAxis("LookUp", this, &AMyCharacter::LookUp);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::CheckJump);
	InputComponent->BindAction("Jump", IE_Released, this, &AMyCharacter::CheckJump);

	InputComponent->BindAction("SwapShoulderView", IE_Pressed, this, &AMyCharacter::SwapShoulderView);
	InputComponent->BindAction("SwitchPerspective", IE_Pressed, this, &AMyCharacter::SwitchPerspective);

}

// Handle character movement and controller rotation
void AMyCharacter::MoveRight(float value) {
	if (value) AddMovementInput(cam->GetRightVector(), value);
}
void AMyCharacter::MoveForward(float value) {
	// If we base this off the camera, the player stops moving when looking down
	if (value) {
		FVector forwardVec = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetScaledAxis(EAxis::X);
		AddMovementInput(forwardVec, value);
	}
}
void AMyCharacter::LookRight(float value) {
	if (value) AddControllerYawInput(value);
}
void AMyCharacter::LookUp(float value) {
	if (value) AddControllerPitchInput(value);
}


void AMyCharacter::CheckJump() {
	if (jumping) jumping = false;
	else jumping = true;
}


// Timelines
void AMyCharacter::TimelineCallback(float val) {
	// Switch shoulder view in third person
	if (bThirdPersonPerspective) {
		float newOffset = bViewingOverRightShoulder ? -75.0f : 75.0f;
		springarm->SocketOffset = FMath::Lerp(springarm->SocketOffset, FVector(0.f, newOffset, 0.f), val);
	}

	// Switch between first and third person
	if (bSwitchingPerspective) {
		springarm->TargetArmLength = FMath::Lerp(springarm->TargetArmLength, bThirdPersonPerspective ? 200.0f : 0.f, val);

		springarm->SocketOffset = FMath::Lerp(springarm->SocketOffset, bThirdPersonPerspective ? FVector(0.f, 75.f, 0.f) : FVector(0.f, 0.f, 0.f), val);

		if (val > 0.5f) {
			GetMesh()->SetHiddenInGame(!bThirdPersonPerspective);
		}
	}
}

void AMyCharacter::TimelineFinishedCallback() {
	bViewingOverRightShoulder = !bViewingOverRightShoulder;
	bSwitchingPerspective = false;
}

void AMyCharacter::PlayTimeline() {
	if (SwapShoulderViewTimeline != NULL) SwapShoulderViewTimeline->PlayFromStart();
}


void AMyCharacter::SwapShoulderView() {
	AMyCharacter::PlayTimeline();
}

void AMyCharacter::SwitchPerspective() {
	bThirdPersonPerspective = !bThirdPersonPerspective;
	FirstPersonArms->SetHiddenInGame(bThirdPersonPerspective);

	UCharacterMovementComponent* const MovementComponent = GetCharacterMovement();

	bSwitchingPerspective = true;
	AMyCharacter::PlayTimeline();
}