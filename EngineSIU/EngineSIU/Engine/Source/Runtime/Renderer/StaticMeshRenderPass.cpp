#include "StaticMeshRenderPass.h"

#include <array>

#include "EngineLoop.h"
#include "World/World.h"

#include "RendererHelpers.h"
#include "UnrealClient.h"

#include "UObject/UObjectIterator.h"
#include "UObject/Casts.h"

#include "D3D11RHI/DXDBufferManager.h"
#include "D3D11RHI/GraphicDevice.h"
#include "D3D11RHI/DXDShaderManager.h"

#include "Components/StaticMeshComponent.h"

#include "BaseGizmos/GizmoBaseComponent.h"
#include "Engine/EditorEngine.h"

#include "PropertyEditor/ShowFlags.h"

#include "UnrealEd/EditorViewportClient.h"


FStaticMeshRenderPass::FStaticMeshRenderPass()
    : VertexShader(nullptr)
    , PixelShader(nullptr)
    , InputLayout(nullptr)
    , BufferManager(nullptr)
    , Graphics(nullptr)
    , ShaderManager(nullptr)
{
}

FStaticMeshRenderPass::~FStaticMeshRenderPass()
{
    ReleaseShader();
}

void FStaticMeshRenderPass::CreateShader()
{
    HRESULT hr = ShaderManager->AddPixelShader(L"StaticMeshPixelShader", L"Shaders/StaticMeshPixelShader.hlsl", "mainPS");
    if (FAILED(hr))
    {
        return;
    }
    hr = ShaderManager->AddPixelShader(L"StaticMeshPixelShaderDepth", L"Shaders/StaticMeshPixelShaderDepth.hlsl", "mainPS");
    if (FAILED(hr))
    {
        return;
    }
    hr = ShaderManager->AddPixelShader(L"StaticMeshPixelShaderWorldNormal", L"Shaders/StaticMeshPixelShaderWorldNormal.hlsl", "mainPS");
    if (FAILED(hr))
    {
        return;
    }
    
    VertexShader = ShaderManager->GetVertexShaderByKey(L"StaticMeshVertexShader");
    InputLayout = ShaderManager->GetInputLayoutByKey(L"StaticMeshVertexShader");
    
    PixelShader = ShaderManager->GetPixelShaderByKey(L"StaticMeshPixelShader");
    DebugDepthShader = ShaderManager->GetPixelShaderByKey(L"StaticMeshPixelShaderDepth");
    DebugWorldNormalShader = ShaderManager->GetPixelShaderByKey(L"StaticMeshPixelShaderWorldNormal");
}

void FStaticMeshRenderPass::ReleaseShader()
{
    
}

void FStaticMeshRenderPass::ChangeViewMode(EViewModeIndex ViewModeIndex) const
{
    switch (ViewModeIndex)
    {
    case EViewModeIndex::VMI_Lit:
        UpdateLitUnlitConstant(1);
        break;
    case EViewModeIndex::VMI_Wireframe:
    case EViewModeIndex::VMI_Unlit:
        UpdateLitUnlitConstant(0);
        break;
    }
}

void FStaticMeshRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    BufferManager = InBufferManager;
    Graphics = InGraphics;
    ShaderManager = InShaderManager;

    CreateShader();
}

void FStaticMeshRenderPass::PrepareRender()
{
    for (const auto iter : TObjectRange<UStaticMeshComponent>())
    {
        if (!Cast<UGizmoBaseComponent>(iter) && iter->GetWorld() == GEngine->ActiveWorld)
        {
            StaticMeshComponents.Add(iter);
        }
    }
}

void FStaticMeshRenderPass::PrepareRenderState(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    const EViewModeIndex ViewMode = Viewport->GetViewMode();
    
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(InputLayout);

    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    TArray<FString> PSBufferKeys = {
        TEXT("FLightBuffer"),
        TEXT("FMaterialConstants"),
        TEXT("FLitUnlitConstants"),
        TEXT("FSubMeshConstants"),
        TEXT("FTextureConstants")
    };

    BufferManager->BindConstantBuffers(PSBufferKeys, 0, EShaderStage::Pixel);

    
    ChangeViewMode(ViewMode);

    // Rasterizer
    if (ViewMode == EViewModeIndex::VMI_Wireframe)
    {
        Graphics->DeviceContext->RSSetState(Graphics->RasterizerWireframeBack);
    }
    else
    {
        Graphics->DeviceContext->RSSetState(Graphics->RasterizerSolidBack);
    }

    // Pixel Shader
    if (ViewMode == EViewModeIndex::VMI_SceneDepth)
    {
        Graphics->DeviceContext->PSSetShader(DebugDepthShader, nullptr, 0);
    }
    else if (ViewMode == EViewModeIndex::VMI_WorldNormal)
    {
        Graphics->DeviceContext->PSSetShader(DebugWorldNormalShader, nullptr, 0);
    }
    else
    {
        Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    }
}

void FStaticMeshRenderPass::UpdateObjectConstant(const FMatrix& WorldMatrix, const FVector4& UUIDColor, bool bIsSelected) const
{
    FObjectConstantBuffer ObjectData = {};
    ObjectData.WorldMatrix = WorldMatrix;
    ObjectData.InverseTransposedWorld = FMatrix::Transpose(FMatrix::Inverse(WorldMatrix));
    ObjectData.UUIDColor = UUIDColor;
    ObjectData.bIsSelected = bIsSelected;
    
    BufferManager->UpdateConstantBuffer(TEXT("FObjectConstantBuffer"), ObjectData);
}

void FStaticMeshRenderPass::UpdateLitUnlitConstant(int32 isLit) const
{
    FLitUnlitConstants Data;
    Data.isLit = isLit;
    BufferManager->UpdateConstantBuffer(TEXT("FLitUnlitConstants"), Data);
}

void FStaticMeshRenderPass::RenderPrimitive(FStaticMeshRenderData* RenderData, TArray<FStaticMaterial*> Materials, TArray<UMaterial*> OverrideMaterials, int SelectedSubMeshIndex) const
{
    UINT Stride = sizeof(FStaticMeshVertex);
    UINT Offset = 0;

    FVertexInfo VertexInfo;
    BufferManager->CreateVertexBuffer(RenderData->ObjectName, RenderData->Vertices, VertexInfo);
    
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &VertexInfo.VertexBuffer, &Stride, &Offset);

    FIndexInfo IndexInfo;
    BufferManager->CreateIndexBuffer(RenderData->ObjectName, RenderData->Indices, IndexInfo);
    if (IndexInfo.IndexBuffer)
    {
        Graphics->DeviceContext->IASetIndexBuffer(IndexInfo.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    }

    if (RenderData->MaterialSubsets.Num() == 0)
    {
        Graphics->DeviceContext->DrawIndexed(RenderData->Indices.Num(), 0, 0);
        return;
    }

    for (int SubMeshIndex = 0; SubMeshIndex < RenderData->MaterialSubsets.Num(); SubMeshIndex++)
    {
        uint32 MaterialIndex = RenderData->MaterialSubsets[SubMeshIndex].MaterialIndex;

        FSubMeshConstants SubMeshData = (SubMeshIndex == SelectedSubMeshIndex) ? FSubMeshConstants(true) : FSubMeshConstants(false);

        BufferManager->UpdateConstantBuffer(TEXT("FSubMeshConstants"), SubMeshData);

        if (OverrideMaterials[MaterialIndex] != nullptr)
        {
            MaterialUtils::UpdateMaterial(BufferManager, Graphics, OverrideMaterials[MaterialIndex]->GetMaterialInfo());
        }
        else
        {
            MaterialUtils::UpdateMaterial(BufferManager, Graphics, Materials[MaterialIndex]->Material->GetMaterialInfo());
        }

        uint32 StartIndex = RenderData->MaterialSubsets[SubMeshIndex].IndexStart;
        uint32 IndexCount = RenderData->MaterialSubsets[SubMeshIndex].IndexCount;
        Graphics->DeviceContext->DrawIndexed(IndexCount, StartIndex, 0);
    }
}

void FStaticMeshRenderPass::RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices) const
{
    UINT Stride = sizeof(FStaticMeshVertex);
    UINT Offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &Offset);
    Graphics->DeviceContext->Draw(numVertices, 0);
}

void FStaticMeshRenderPass::RenderPrimitive(ID3D11Buffer* pVertexBuffer, UINT numVertices, ID3D11Buffer* pIndexBuffer, UINT numIndices) const
{
    UINT Stride = sizeof(FStaticMeshVertex);
    UINT Offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &Stride, &Offset);
    Graphics->DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    Graphics->DeviceContext->DrawIndexed(numIndices, 0, 0);
}

void FStaticMeshRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    const EResourceType ResourceType = EResourceType::ERT_Scene;
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(ResourceType);
    
    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, ViewportResource->GetDepthStencilView());
    ViewportResource->ClearRenderTarget(Graphics->DeviceContext, ResourceType);
    Graphics->DeviceContext->ClearDepthStencilView(ViewportResource->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    PrepareRenderState(Viewport);

    for (UStaticMeshComponent* Comp : StaticMeshComponents)
    {
        if (!Comp || !Comp->GetStaticMesh())
        {
            continue;
        }

        FStaticMeshRenderData* RenderData = Comp->GetStaticMesh()->GetRenderData();
        if (RenderData == nullptr)
        {
            continue;
        }
        
        UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
        
        FMatrix WorldMatrix = Comp->GetWorldMatrix();
        FVector4 UUIDColor = Comp->EncodeUUID() / 255.0f;
        const bool bIsSelected = (Engine && Engine->GetSelectedActor() == Comp->GetOwner());
        
        UpdateObjectConstant(WorldMatrix, UUIDColor, bIsSelected);
        
        RenderPrimitive(RenderData, Comp->GetStaticMesh()->GetMaterials(), Comp->GetOverrideMaterials(), Comp->GetselectedSubMeshIndex());

        if (Viewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_AABB))
        {
            FEngineLoop::PrimitiveDrawBatch.AddAABBToBatch(Comp->GetBoundingBox(), Comp->GetWorldLocation(), WorldMatrix);
        }
    }

    // 렌더 타겟 해제
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}

void FStaticMeshRenderPass::ClearRenderArr()
{
    StaticMeshComponents.Empty();
}

