#include "BlurEffect.h"
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")

// Embedded shader source (compiled at runtime)
static const char* g_blurVS = R"HLSL(
struct VS_INPUT {
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.pos = float4(input.pos, 1.0f);
    output.uv = input.uv;
    return output;
}
)HLSL";

static const char* g_blurPS = R"HLSL(
Texture2D inputTexture : register(t0);
SamplerState linearSampler : register(s0);

cbuffer BlurParams : register(b0) {
    float2 texelSize;
    float2 direction;
    float blurRadius;
    float3 padding;
};

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

static const float weights[5] = {
    0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f
};

float4 main(PS_INPUT input) : SV_TARGET {
    float2 uv = input.uv;
    float3 result = inputTexture.Sample(linearSampler, uv).rgb * weights[0];

    float2 offset = direction * texelSize * blurRadius;

    for (int i = 1; i < 5; i++) {
        float2 sampleOffset = offset * float(i);
        result += inputTexture.Sample(linearSampler, uv + sampleOffset).rgb * weights[i];
        result += inputTexture.Sample(linearSampler, uv - sampleOffset).rgb * weights[i];
    }

    return float4(result, 1.0f);
}
)HLSL";

bool BlurEffect::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height) {
    m_device = device;
    m_context = context;
    m_width = width;
    m_height = height;

    if (!CreateShaders()) {
        return false;
    }

    if (!CreateResources(width, height)) {
        return false;
    }

    m_initialized = true;
    return true;
}

bool BlurEffect::CreateShaders() {
    HRESULT hr;
    ComPtr<ID3DBlob> vsBlob, psBlob, errorBlob;

    // Compile vertex shader
    hr = D3DCompile(g_blurVS, strlen(g_blurVS), "BlurVS", nullptr, nullptr,
                    "main", "vs_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0,
                    &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        return false;
    }

    hr = m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
                                       nullptr, &m_vertexShader);
    if (FAILED(hr)) return false;

    // Create input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = m_device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(),
                                      vsBlob->GetBufferSize(), &m_inputLayout);
    if (FAILED(hr)) return false;

    // Compile pixel shader
    hr = D3DCompile(g_blurPS, strlen(g_blurPS), "BlurPS", nullptr, nullptr,
                    "main", "ps_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0,
                    &psBlob, &errorBlob);
    if (FAILED(hr)) {
        return false;
    }

    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
                                      nullptr, &m_pixelShader);
    if (FAILED(hr)) return false;

    // Create constant buffer
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(BlurParams);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = m_device->CreateBuffer(&cbDesc, nullptr, &m_constantBuffer);
    if (FAILED(hr)) return false;

    // Create sampler
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    hr = m_device->CreateSamplerState(&samplerDesc, &m_sampler);
    if (FAILED(hr)) return false;

    // Create fullscreen quad vertex buffer
    struct Vertex {
        float x, y, z;
        float u, v;
    };

    Vertex quadVertices[] = {
        { -1.0f,  1.0f, 0.0f, 0.0f, 0.0f },
        {  1.0f,  1.0f, 0.0f, 1.0f, 0.0f },
        { -1.0f, -1.0f, 0.0f, 0.0f, 1.0f },
        {  1.0f, -1.0f, 0.0f, 1.0f, 1.0f }
    };

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(quadVertices);
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = quadVertices;

    hr = m_device->CreateBuffer(&vbDesc, &vbData, &m_quadVB);
    return SUCCEEDED(hr);
}

bool BlurEffect::CreateResources(int width, int height) {
    ReleaseResources();

    m_width = width;
    m_height = height;

    // Create two render targets for ping-pong blur
    for (int i = 0; i < 2; i++) {
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        HRESULT hr = m_device->CreateTexture2D(&texDesc, nullptr, &m_blurTexture[i]);
        if (FAILED(hr)) return false;

        hr = m_device->CreateRenderTargetView(m_blurTexture[i].Get(), nullptr, &m_blurRTV[i]);
        if (FAILED(hr)) return false;

        hr = m_device->CreateShaderResourceView(m_blurTexture[i].Get(), nullptr, &m_blurSRV[i]);
        if (FAILED(hr)) return false;
    }

    return true;
}

void BlurEffect::ReleaseResources() {
    for (int i = 0; i < 2; i++) {
        m_blurSRV[i].Reset();
        m_blurRTV[i].Reset();
        m_blurTexture[i].Reset();
    }
}

void BlurEffect::Resize(int width, int height) {
    if (m_initialized && (width != m_width || height != m_height)) {
        CreateResources(width, height);
    }
}

void BlurEffect::Shutdown() {
    ReleaseResources();
    m_quadVB.Reset();
    m_sampler.Reset();
    m_constantBuffer.Reset();
    m_inputLayout.Reset();
    m_pixelShader.Reset();
    m_vertexShader.Reset();
    m_initialized = false;
}

ID3D11ShaderResourceView* BlurEffect::Apply(ID3D11ShaderResourceView* srcSRV, float blurStrength) {
    if (!m_initialized || !srcSRV) return nullptr;

    // Save current state
    ComPtr<ID3D11RenderTargetView> oldRTV;
    ComPtr<ID3D11DepthStencilView> oldDSV;
    m_context->OMGetRenderTargets(1, &oldRTV, &oldDSV);

    D3D11_VIEWPORT oldVP;
    UINT numVPs = 1;
    m_context->RSGetViewports(&numVPs, &oldVP);

    // Set viewport
    D3D11_VIEWPORT vp = {};
    vp.Width = (float)m_width;
    vp.Height = (float)m_height;
    vp.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &vp);

    // Set common state
    m_context->IASetInputLayout(m_inputLayout.Get());
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    UINT stride = sizeof(float) * 5;
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, m_quadVB.GetAddressOf(), &stride, &offset);
    m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    m_context->PSSetSamplers(0, 1, m_sampler.GetAddressOf());

    // Multiple blur passes for stronger effect
    int numPasses = (int)(blurStrength * 2);
    if (numPasses < 2) numPasses = 2;

    ID3D11ShaderResourceView* currentSRV = srcSRV;
    int finalTarget = 0;

    for (int pass = 0; pass < numPasses; pass++) {
        // Horizontal pass
        {
            float clearColor[] = { 0, 0, 0, 1 };
            m_context->ClearRenderTargetView(m_blurRTV[0].Get(), clearColor);
            m_context->OMSetRenderTargets(1, m_blurRTV[0].GetAddressOf(), nullptr);

            D3D11_MAPPED_SUBRESOURCE mapped;
            if (SUCCEEDED(m_context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
                BlurParams* params = (BlurParams*)mapped.pData;
                params->texelSizeX = 1.0f / m_width;
                params->texelSizeY = 1.0f / m_height;
                params->directionX = 1.0f;
                params->directionY = 0.0f;
                params->blurRadius = blurStrength;
                m_context->Unmap(m_constantBuffer.Get(), 0);
            }

            m_context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
            m_context->PSSetShaderResources(0, 1, &currentSRV);
            m_context->Draw(4, 0);

            ID3D11ShaderResourceView* nullSRV = nullptr;
            m_context->PSSetShaderResources(0, 1, &nullSRV);
        }

        // Vertical pass
        {
            float clearColor[] = { 0, 0, 0, 1 };
            m_context->ClearRenderTargetView(m_blurRTV[1].Get(), clearColor);
            m_context->OMSetRenderTargets(1, m_blurRTV[1].GetAddressOf(), nullptr);

            D3D11_MAPPED_SUBRESOURCE mapped;
            if (SUCCEEDED(m_context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
                BlurParams* params = (BlurParams*)mapped.pData;
                params->texelSizeX = 1.0f / m_width;
                params->texelSizeY = 1.0f / m_height;
                params->directionX = 0.0f;
                params->directionY = 1.0f;
                params->blurRadius = blurStrength;
                m_context->Unmap(m_constantBuffer.Get(), 0);
            }

            m_context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
            m_context->PSSetShaderResources(0, 1, m_blurSRV[0].GetAddressOf());
            m_context->Draw(4, 0);

            ID3D11ShaderResourceView* nullSRV = nullptr;
            m_context->PSSetShaderResources(0, 1, &nullSRV);
        }

        currentSRV = m_blurSRV[1].Get();
        finalTarget = 1;
    }

    // Restore state
    m_context->OMSetRenderTargets(1, oldRTV.GetAddressOf(), oldDSV.Get());
    m_context->RSSetViewports(1, &oldVP);

    return m_blurSRV[finalTarget].Get();
}
