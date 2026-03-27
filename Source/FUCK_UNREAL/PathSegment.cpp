#include "PathSegment.h"

APathSegment::APathSegment()
{
	PrimaryActorTick.bCanEverTick = false;
}

FVector APathSegment::GetClosestPoint(const FVector& PlayerPos) const
{
	if (!PointA || !PointB) return PlayerPos;

	FVector A = PointA->GetActorLocation();
	FVector B = PointB->GetActorLocation();

	// flatten to XY
	A.Z = 0;
	B.Z = 0;
	FVector P = FVector(PlayerPos.X, PlayerPos.Y, 0);

	FVector AB = B - A;
	FVector AP = P - A;

	float T = FVector::DotProduct(AP, AB) / FVector::DotProduct(AB, AB);
	T = FMath::Clamp(T, 0.f, 1.f);

	FVector Closest = A + AB * T;
	Closest.Z = PlayerPos.Z; // restore actual height

	return Closest;
}

bool APathSegment::IsOutside(const FVector& PlayerPos) const
{
	FVector Closest = GetClosestPoint(PlayerPos);

	FVector FlatPlayer = FVector(PlayerPos.X, PlayerPos.Y, 0);
	FVector FlatClosest = FVector(Closest.X, Closest.Y, 0);

	return FVector::Dist(FlatPlayer, FlatClosest) > Radius;
}

FVector APathSegment::GetOutwardDirection(const FVector& PlayerPos) const
{
	FVector Closest = GetClosestPoint(PlayerPos);

	FVector Dir = FVector(PlayerPos.X - Closest.X, PlayerPos.Y - Closest.Y, 0);
	return Dir.GetSafeNormal();
}