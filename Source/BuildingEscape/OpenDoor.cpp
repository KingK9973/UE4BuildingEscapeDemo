// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TriggerVolume.h"
#include "Engine/Classes/Components/PrimitiveComponent.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	//ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();

	TArray<AActor*> FetchedActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATriggerVolume::StaticClass(), FetchedActors);
	for (AActor* TriggerVolume : FetchedActors) {
		if (TriggerVolume->GetName().Equals("TriggerVolumeOpenDoor")) {
			PressurePlate = Cast<ATriggerVolume>(TriggerVolume);
		}
	}
}

void UOpenDoor::OpenCloseDoor(float Yaw) {
	AActor* Owner = GetOwner();
	FRotator NewRotation = FRotator(0.f, Yaw, 0.f);

	Owner->SetActorRotation(NewRotation);
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate) {
		if (GetToatalMassOfActorOnPlate() > 20.0f) {
			OpenCloseDoor(OpenAngle);
			LastDoorOpenTime = GetWorld()->GetTimeSeconds();
		}
		if (GetWorld()->GetTimeSeconds() - LastDoorOpenTime > DoorCloseDelay) {
			OpenCloseDoor(0.0f);
		}
	}
}

float UOpenDoor::GetToatalMassOfActorOnPlate()
{
	float TotalMass = 0.0f;
	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	for (auto& actor : OverlappingActors) {
		TotalMass += actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	//UE_LOG(LogTemp, Warning, TEXT("Total Mass = %s ..."), *FString::Printf(TEXT("%f"), TotalMass))

	return TotalMass;
}
