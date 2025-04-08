#include "HeightFogComponent.h"

UHeightFogComponent::UHeightFogComponent(float Density, float HeightFalloff, float StartDist, float CutoffDist, float MaxOpacity)
    :FogDensity(Density), FogHeightFalloff(HeightFalloff), StartDistance(StartDist), FogCutoffDistance(CutoffDist), FogMaxOpacity(MaxOpacity)
{
    FogInscatteringColor = FLinearColor::White;
    LightDirection = FVector(1, -1, -1).GetSafeNormal();
    InScatteringIntensity = 1;
}

void UHeightFogComponent::SetFogDensity(float value)
{
    FogDensity = value;
}

void UHeightFogComponent::SetFogHeightFalloff(float value)
{
    FogHeightFalloff = value; 
}

void UHeightFogComponent::SetStartDistance(float value)
{
    StartDistance = value;
}

void UHeightFogComponent::SetFogCutoffDistance(float value)
{
    FogCutoffDistance = value;
}

void UHeightFogComponent::SetFogMaxOpacity(float value)
{
    FogMaxOpacity = value;
}

void UHeightFogComponent::SetFogColor(FLinearColor color)
{
    FogInscatteringColor = color;
}

void UHeightFogComponent::SetLightDirection(FVector direction)
{
    LightDirection = direction;
}

void UHeightFogComponent::SetInScatteringIntensity(float intensity)
{
    InScatteringIntensity = intensity;
}
