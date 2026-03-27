#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathSegment.generated.h"

UCLASS()
class FUCK_UNREAL_API APathSegment : public AActor
{
	GENERATED_BODY()

public:
	APathSegment();

	UPROPERTY(EditAnywhere, Category = "Path")
	AActor* PointA;

	UPROPERTY(EditAnywhere, Category = "Path")
	AActor* PointB;

	UPROPERTY(EditAnywhere, Category = "Path")
	float Radius = 300.f;

	// returns closest point on segment line to given position (XY only)
	FVector GetClosestPoint(const FVector& PlayerPos) const;

	// returns true if player is outside radius
	bool IsOutside(const FVector& PlayerPos) const;

	// returns the outward direction from segment to player (XY only, normalized)
	FVector GetOutwardDirection(const FVector& PlayerPos) const;
};