#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>
#include <memory>
#include "Math/Color.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Engine/Classes/Components/HeightFogComponent.h"
class FGraphicsDevice;
class FDXDShaderManager;
class FDXDBufferManager;
class FEditorViewportClient;

class FFogRenderPass
{
public:
    FFogRenderPass();
    ~FFogRenderPass();

    // 초기화: 그래픽 디바이스와 셰이더 매니저를 등록
    void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager);

    // 스프라이트 렌더링에 필요한 버퍼 및 상태 생성
    void CreateSpriteResources();

    // Fog 렌더링용 셰이더 생성 및 입력 레이아웃 설정
    void CreateShader();

    void CreateSceneSrv();

    void PrepareRender();

    void PrepareRenderState(ID3D11ShaderResourceView* DepthSRV);
    // Fog를 화면에 렌더링
    void RenderFog(const std::shared_ptr<FEditorViewportClient>& ActiveViewport, ID3D11ShaderResourceView* DepthSRV, TArray< UHeightFogComponent*> Fogs);

    void CheckResize();

    void UpdateScreenConstant(const D3D11_VIEWPORT& viewport);

    void UpdateFogConstant(const std::shared_ptr<FEditorViewportClient>& ActiveViewport, UHeightFogComponent* Fog);

    void CreateBlendState();

    void PrepareFinalRender();

    void FinalRender();

    void CreateRTV();

private:
    ID3D11SamplerState* Sampler = nullptr;

    FGraphicsDevice* Graphics;
    FDXDBufferManager* BufferManager;
    FDXDShaderManager* ShaderManager;

    // 전체 화면 Quad 렌더링용 버퍼
    ID3D11Buffer* FogVertexBuffer;
    ID3D11Buffer* FogIndexBuffer;

    // Fog 렌더링용 셰이더 및 입력 레이아웃
    ID3D11VertexShader* FogVertexShader;
    ID3D11PixelShader* FogPixelShader;
    ID3D11PixelShader* FogQuadPixelShader;
    ID3D11InputLayout* InputLayout;

    // Scene SRV (외부에서 등록)
    ID3D11ShaderResourceView* SceneSRV;
    ID3D11ShaderResourceView* FogSRV;

    ID3D11Texture2D* FogBuffer = nullptr;
    ID3D11RenderTargetView* FogRTV = nullptr;

    ID3D11BlendState* FogBlendState = nullptr;

    TArray<UHeightFogComponent*> FogComponents;

    bool bRender = false;
    float screenWidth = 0;
    float screenHeight = 0;
};