#include "DX11Context.h"
#include <stdexcept>

DX11Context::~DX11Context() {
    Cleanup();
}

bool DX11Context::Initialize(HWND hwnd, int width, int height) {
    m_width = width;
    m_height = height;

    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevelArray,
        _countof(featureLevelArray),
        D3D11_SDK_VERSION,
        &sd,
        m_swapChain.GetAddressOf(),
        m_device.GetAddressOf(),
        &featureLevel,
        m_context.GetAddressOf()
    );

    if (FAILED(hr)) {
        return false;
    }

    if (!CreateRenderTarget()) {
        return false;
    }

    return true;
}

void DX11Context::Cleanup() {
    CleanupRenderTarget();

    if (m_swapChain) {
        m_swapChain->SetFullscreenState(FALSE, nullptr);
    }

    m_swapChain.Reset();
    m_context.Reset();
    m_device.Reset();
}

bool DX11Context::CreateRenderTarget() {
    ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (FAILED(hr)) {
        return false;
    }

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
    return SUCCEEDED(hr);
}

void DX11Context::CleanupRenderTarget() {
    m_backbufferCopySRV.Reset();
    m_backbufferCopy.Reset();
    m_renderTargetView.Reset();
}

void DX11Context::BeginFrame(float r, float g, float b, float a) {
    const float clearColor[4] = { r, g, b, a };
    m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);
    m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
}

void DX11Context::EndFrame() {
    m_swapChain->Present(1, 0); // VSync on
}

void DX11Context::Resize(int width, int height) {
    if (width == 0 || height == 0) {
        return;
    }

    // Safety check - ensure DX11 is initialized
    if (!m_swapChain || !m_device || !m_context) {
        return;
    }

    m_width = width;
    m_height = height;

    // Flush any pending GPU work
    m_context->ClearState();
    m_context->Flush();

    CleanupRenderTarget();

    HRESULT hr = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) {
        return;
    }

    CreateRenderTarget();
}

ID3D11ShaderResourceView* DX11Context::CopyBackbuffer() {
    // Get backbuffer
    ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (FAILED(hr)) return nullptr;

    // Create copy texture if needed
    if (!m_backbufferCopy) {
        D3D11_TEXTURE2D_DESC desc;
        backBuffer->GetDesc(&desc);
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        hr = m_device->CreateTexture2D(&desc, nullptr, &m_backbufferCopy);
        if (FAILED(hr)) return nullptr;

        hr = m_device->CreateShaderResourceView(m_backbufferCopy.Get(), nullptr, &m_backbufferCopySRV);
        if (FAILED(hr)) return nullptr;
    }

    // Copy backbuffer to our texture
    m_context->CopyResource(m_backbufferCopy.Get(), backBuffer.Get());

    return m_backbufferCopySRV.Get();
}
