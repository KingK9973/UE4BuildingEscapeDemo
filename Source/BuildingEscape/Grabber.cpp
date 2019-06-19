// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h" 
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Engine/Classes/Components/PrimitiveComponent.h"
#include "Engine/EngineTypes.h"

#define OUT


UGrabber::UGrabber() {
	PrimaryComponentTick.bCanEverTick = true;
}


void UGrabber::BeginPlay() {
	Super::BeginPlay();
	
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *GetOwner()->GetName())
	}

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		UE_LOG(LogTemp, Warning, TEXT("Input Component Found"))

		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s missing Input Component component"), *GetOwner()->GetName())
	}
		
}

void UGrabber::Grab() {
	auto HitResult = GetFirstPhysicsBodyInReach();
	ComponentToGrab = HitResult.GetComponent();
	ActorHit = HitResult.GetActor();
	if (ComponentToGrab && ActorHit && PhysicsHandle) {
		PhysicsHandle->GrabComponent(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(), true);

		ComponentToGrab->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}
}

void UGrabber::Release() {
	if (PhysicsHandle) {
		PhysicsHandle->ReleaseComponent();
	}
		if (ComponentToGrab && ActorHit) {
			ComponentToGrab->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		}
}



void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {		// Called every frame
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (PhysicsHandle && PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}
}

FVector UGrabber::GetReachLineEnd() {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);
	return  PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);
}

FVector UGrabber::GetReachLineStart() {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);
	return  PlayerViewPointLocation;
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach() {

	//void DrawDebugLine(const UWorld * InWorld, FVector const & LineStart, FVector const & LineEnd, FColor const & Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness);
	DrawDebugLine(GetWorld(), GetReachLineStart(), GetReachLineEnd(), FColor(255, 0, 0), false, 0.0f, 0.0f, 20.0f);

	FHitResult HitResult;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(OUT HitResult, GetReachLineStart(), GetReachLineEnd(), FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParameters);

	AActor* ActorHit = HitResult.GetActor();
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Line Trace Hit:  %s ..."), *(ActorHit->GetName()));
	}

	return HitResult;
}