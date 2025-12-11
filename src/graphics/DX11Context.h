#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <string>

using Microsoft::WRL::ComPtr;

class DX11Context {
public:
    DX11Context() = default;
    ~DX11Context();

    bool Initialize(HWND hwnd, int width, int height);
    void Cleanup();

    void BeginFrame(float r = 0.1f, float g = 0.1f, float b = 0.15f, float a = 1.0f);
    void EndFrame();

    void Resize(int width, int height);

    // Getters
    ID3D11Device* GetDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* GetContext() const { return m_context.Get(); }
    IDXGISwapChain* GetSwapChain() const { return m_swapChain.Get(); }
    ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView.Get(); }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    // Copy backbuffer to a texture for effects
    ID3D11ShaderResourceView* CopyBackbuffer();
    ID3D11ShaderResourceView* GetBackbufferCopySRV() const { return m_backbufferCopySRV.Get(); }

private:
    bool CreateRenderTarget();
    void CleanupRenderTarget();

    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;

    // Backbuffer copy for effects
    ComPtr<ID3D11Texture2D> m_backbufferCopy;
    ComPtr<ID3D11ShaderResourceView> m_backbufferCopySRV;

    int m_width = 0;
    int m_height = 0;
};
