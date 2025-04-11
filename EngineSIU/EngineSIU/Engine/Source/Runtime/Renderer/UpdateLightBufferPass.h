#pragma once

#include "IRenderPass.h"
#include "Define.h"

class FDXDShaderManager;
class UWorld;
class FEditorViewportClient;

class UPointLightComponent;
class USpotLightComponent;

class FUpdateLightBufferPass : public IRenderPass
{
public:
    FUpdateLightBufferPass();
    ~FUpdateLightBufferPass();

    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;
    virtual void PrepareRender() override;
    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void ClearRenderArr() override;
    void UpdateLightBuffer(FLight Light) const;

private:
    TArray<USpotLightComponent*> SpotLights;
    TArray<UPointLightComponent*> PointLights;

    FDXDBufferManager* BufferManager;
    FGraphicsDevice* Graphics;
    FDXDShaderManager* ShaderManager;
};
