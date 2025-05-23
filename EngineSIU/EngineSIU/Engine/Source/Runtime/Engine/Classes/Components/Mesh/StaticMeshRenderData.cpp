#include "StaticMeshRenderData.h"
#include "Engine/FObjLoader.h"
#include "UObject/Casts.h"
#include "UObject/ObjectFactory.h"

#include "Engine/Asset/StaticMeshAsset.h"

UObject* UStaticMesh::Duplicate(UObject* InOuter)
{
    // TODO: Context->CopyResource를 사용해서 Buffer복사
    // ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate());
    return nullptr;
}

uint32 UStaticMesh::GetMaterialIndex(FName MaterialSlotName) const
{
    for (uint32 materialIndex = 0; materialIndex < materials.Num(); materialIndex++) {
        if (materials[materialIndex]->MaterialSlotName == MaterialSlotName)
            return materialIndex;
    }

    return -1;
}

void UStaticMesh::GetUsedMaterials(TArray<UMaterial*>& Out) const
{
    for (const FStaticMaterial* Material : materials)
    {
        Out.Emplace(Material->Material);
    }
}

FWString UStaticMesh::GetObjectName() const
{
    return staticMeshRenderData->ObjectName;
}

void UStaticMesh::SetData(FStaticMeshRenderData* renderData)
{
    staticMeshRenderData = renderData;

    for (int materialIndex = 0; materialIndex < staticMeshRenderData->Materials.Num(); materialIndex++) {
        FStaticMaterial* newMaterialSlot = new FStaticMaterial();
        UMaterial* newMaterial = FObjManager::CreateMaterial(staticMeshRenderData->Materials[materialIndex]);

        newMaterialSlot->Material = newMaterial;
        newMaterialSlot->MaterialSlotName = staticMeshRenderData->Materials[materialIndex].MaterialName;

        materials.Add(newMaterialSlot);
    }
}
