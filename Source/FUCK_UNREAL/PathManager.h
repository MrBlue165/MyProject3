#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathManager.generated.h"

class APathSegment;
class APathPoint;

UCLASS()
class FUCK_UNREAL_API APathManager : public AActor
{
	GENERATED_BODY()

public:
	APathManager();

	UPROPERTY(EditAnywhere, Category = "Path")
	float SegmentRadius = 30.f;

	UPROPERTY()
	TArray<APathSegment*> Segments;

	virtual void BeginPlay() override;

	APathSegment* GetClosestSegment(const FVector& Pos) const;

private:
	void BuildSegments(TMap<FString, TArray<APathPoint*>>& Paths);
	void SpawnSegment(AActor* A, AActor* B);
};