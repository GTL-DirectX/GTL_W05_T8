#include "CubeComp.h"
#include "Engine/FLoaderOBJ.h"
#include "UObject/ObjectFactory.h"
#include "UObject/Casts.h"


UCubeComp::UCubeComp()
{
    AABB.max = { 1,1,1 };
    AABB.min = { -1,-1,-1 };

}

void UCubeComp::InitializeComponent()
{
    Super::InitializeComponent();

    // TODO: Create는 AssetManager가 처리하도록 변경.
    FManagerOBJ::CreateStaticMesh("Assets/helloBlender.obj");
    SetStaticMesh(FManagerOBJ::GetStaticMesh(L"helloBlender.obj"));
}

void UCubeComp::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

}

UObject* UCubeComp::Duplicate()
{
    UCubeComp* NewComponent = Cast<UCubeComp>(Super::Duplicate());
    if (!NewComponent)
        return nullptr;
    NewComponent->AABB = AABB;
    NewComponent->SetStaticMesh(GetStaticMesh());
    return NewComponent;
}
