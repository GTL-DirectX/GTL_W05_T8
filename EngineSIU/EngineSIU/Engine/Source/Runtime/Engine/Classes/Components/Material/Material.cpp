#include "Material.h"

#include "UObject/Casts.h"

UObject* UMaterial::Duplicate()
{
    UMaterial* NewMaterial = Cast<UMaterial>(Super::Duplicate());
    if (!NewMaterial)
        return nullptr;
    NewMaterial->materialInfo = materialInfo;
    return NewMaterial;
}
