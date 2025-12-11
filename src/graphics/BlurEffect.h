#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class BlurEffect {
public:
    BlurEffect() = default;
    ~BlurEffect() = default;

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);
    void Resize(int width, int height);
    void Shutdown();

    // Blur the source texture and return the result
    // srcSRV: source texture to blur
    // region: the area to blur (in screen coordinates)
    // blurStrength: how much to blur (1.0 = normal, higher = more blur)
    ID3D11ShaderResourceView* Apply(
        ID3D11ShaderResourceView* srcSRV,
        float blurStrength = 1.5f
    );

    // Get the blurred texture for rendering
    ID3D11ShaderResourceView* GetBlurredSRV() const { return m_blurSRV[1].Get(); }

private:
    bool CreateShaders();
    bool CreateResources(int width, int height);
    void ReleaseResources();

    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;

    // Shaders
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;

    // Constant buffer for blur parameters
    ComPtr<ID3D11Buffer> m_constantBuffer;

    // Two render targets for ping-pong blur
    ComPtr<ID3D11Texture2D> m_blurTexture[2];
    ComPtr<ID3D11RenderTargetView> m_blurRTV[2];
    ComPtr<ID3D11ShaderResourceView> m_blurSRV[2];

    // Fullscreen quad
    ComPtr<ID3D11Buffer> m_quadVB;

    // Sampler
    ComPtr<ID3D11SamplerState> m_sampler;

    int m_width = 0;
    int m_height = 0;
    bool m_initialized = false;

    struct BlurParams {
        float texelSizeX;
        float texelSizeY;
        float directionX;
        float directionY;
        float blurRadius;
        float padding[3];
    };
};
