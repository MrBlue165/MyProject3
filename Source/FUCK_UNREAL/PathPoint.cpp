#include "PathPoint.h"
#include "Kismet/GameplayStatics.h"

APathPoint::APathPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APathPoint::AutoAssignIndex()
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APathPoint::StaticClass(), Found);

	int32 HighestIndex = -1;

	for (AActor* A : Found)
	{
		APathPoint* P = Cast<APathPoint>(A);
		if (!P || P == this) continue;
		if (P->PathName != PathName) continue;

		if (P->Index > HighestIndex)
		{
			HighestIndex = P->Index;
		}
	}

	Index = HighestIndex + 1;
}