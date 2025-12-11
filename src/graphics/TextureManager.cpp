#include "TextureManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TextureManager& TextureManager::Instance() {
    static TextureManager instance;
    return instance;
}

void TextureManager::Initialize(ID3D11Device* device) {
    m_device = device;
}

void TextureManager::Shutdown() {
    ClearAll();
    m_device = nullptr;
}

Texture* TextureManager::LoadTexture(const std::string& path) {
    // Check if already loaded
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return &it->second;
    }

    if (!m_device) {
        return nullptr;
    }

    // Load image data
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

    if (!data) {
        return nullptr;
    }

    // Create texture
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;
    initData.SysMemPitch = width * 4;

    Texture texture;
    texture.width = width;
    texture.height = height;

    HRESULT hr = m_device->CreateTexture2D(&desc, &initData, texture.texture.GetAddressOf());

    stbi_image_free(data);

    if (FAILED(hr)) {
        return nullptr;
    }

    // Create shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = m_device->CreateShaderResourceView(texture.texture.Get(), &srvDesc, texture.srv.GetAddressOf());

    if (FAILED(hr)) {
        return nullptr;
    }

    // Store and return
    m_textures[path] = std::move(texture);
    return &m_textures[path];
}

Texture* TextureManager::GetTexture(const std::string& path) {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return &it->second;
    }
    return nullptr;
}

void TextureManager::UnloadTexture(const std::string& path) {
    m_textures.erase(path);
}

void TextureManager::ClearAll() {
    m_textures.clear();
}
