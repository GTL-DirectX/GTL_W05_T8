#include "SkySphereComponent.h"
#include "UObject/Casts.h"


USkySphereComponent::USkySphereComponent()
{
}

void USkySphereComponent::TickComponent(float DeltaTime)
{
    UOffset += 0.005f;
    VOffset += 0.005f;
    Super::TickComponent(DeltaTime);
}

UObject* USkySphereComponent::Duplicate()
{
    USkySphereComponent* NewComponent = Cast<USkySphereComponent>(Super::Duplicate());
    if (!NewComponent)
        return nullptr;
    NewComponent->UOffset = UOffset;
    NewComponent->VOffset = VOffset;
    return NewComponent;
}
