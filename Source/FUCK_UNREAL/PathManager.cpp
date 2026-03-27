#include "PathManager.h"
#include "PathSegment.h"
#include "PathPoint.h"
#include "Kismet/GameplayStatics.h"

APathManager::APathManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APathManager::BeginPlay()
{
    Super::BeginPlay();

    // gather all path points
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APathPoint::StaticClass(), Found);

    // group by path name
    TMap<FString, TArray<APathPoint*>> Paths;
    for (AActor* A : Found)
    {
        APathPoint* P = Cast<APathPoint>(A);
        if (!P) continue;
        Paths.FindOrAdd(P->PathName).Add(P);
    }

    // sort each path by index
    for (auto& Pair : Paths)
    {
        Pair.Value.Sort([](const APathPoint& A, const APathPoint& B)
        {
            return A.Index < B.Index;
        });
    }

    BuildSegments(Paths);
}

void APathManager::BuildSegments(TMap<FString, TArray<APathPoint*>>& Paths)
{
    for (auto& Pair : Paths)
    {
        TArray<APathPoint*>& Points = Pair.Value;

        for (int i = 0; i < Points.Num(); i++)
        {
            APathPoint* Current = Points[i];

            // connect to next point on same path
            if (i + 1 < Points.Num())
            {
                SpawnSegment(Current, Points[i + 1]);
            }

            // connect to first point of each connected path
            for (const FString& ConnectedName : Current->ConnectedPaths)
            {
                if (!Paths.Contains(ConnectedName)) continue;
                TArray<APathPoint*>& OtherPath = Paths[ConnectedName];
                if (OtherPath.Num() == 0) continue;
                SpawnSegment(Current, OtherPath[0]);
            }
        }
    }
}

void APathManager::SpawnSegment(AActor* A, AActor* B)
{
    APathSegment* Seg = GetWorld()->SpawnActor<APathSegment>(
        APathSegment::StaticClass(),
        FTransform::Identity
    );

    if (Seg)
    {
        Seg->PointA = A;
        Seg->PointB = B;
        Seg->Radius = SegmentRadius;
        Segments.Add(Seg);
    }
}

APathSegment* APathManager::GetClosestSegment(const FVector& Pos) const
{
    APathSegment* Best = nullptr;
    float BestDist = FLT_MAX;

    for (APathSegment* Seg : Segments)
    {
        if (!Seg) continue;

        FVector Closest = Seg->GetClosestPoint(Pos);
        float Dist = FVector::Dist2D(FVector(Pos.X, Pos.Y, 0), FVector(Closest.X, Closest.Y, 0));

        if (Dist < BestDist)
        {
            BestDist = Dist;
            Best = Seg;
        }
    }

    return Best;
}