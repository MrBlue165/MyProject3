#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathPoint.generated.h"

UCLASS()
class FUCK_UNREAL_API APathPoint : public AActor
{
	GENERATED_BODY()

public:
	APathPoint();

	UPROPERTY(EditAnywhere, Category = "Path")
	FString PathName = "Default";

	UPROPERTY(EditAnywhere, Category = "Path")
	int32 Index = 0;

	UPROPERTY(EditAnywhere, Category = "Path")
	TArray<FString> ConnectedPaths;

	// button in editor that scans all PathPoints with same name and sets index to highest+1
	UFUNCTION(CallInEditor, Category = "Path")
	void AutoAssignIndex();
};