#pragma once
#include "Engine/Source/Runtime/Engine/Classes/Components/SceneComponent.h"

class UPrimitiveComponent : public USceneComponent
{
    DECLARE_CLASS(UPrimitiveComponent, USceneComponent)

public:
    UPrimitiveComponent();
    virtual ~UPrimitiveComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual UObject* Duplicate() override;
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance) override;
    bool IntersectRayTriangle(
        const FVector& rayOrigin, const FVector& rayDirection,
        const FVector& v0, const FVector& v1, const FVector& v2, float& hitDistance
    ) const;
    FBoundingBox AABB;

public:
    FBoundingBox GetBoundingBox() const { return AABB; }
};

