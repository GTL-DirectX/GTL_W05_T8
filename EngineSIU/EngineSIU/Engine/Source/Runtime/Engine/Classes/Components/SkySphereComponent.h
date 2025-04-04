#pragma once
#include "Components/StaticMeshComponent.h"


class USkySphereComponent : public UStaticMeshComponent
{
    DECLARE_CLASS(USkySphereComponent, UStaticMeshComponent)

public:
    USkySphereComponent();

    virtual void TickComponent(float DeltaTime) override;
    virtual UObject* Duplicate() override;

    float UOffset = 0;
    float VOffset = 0;
};
