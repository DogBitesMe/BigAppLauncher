#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>

using Microsoft::WRL::ComPtr;

struct Texture {
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> srv;
    int width = 0;
    int height = 0;
};

class TextureManager {
public:
    static TextureManager& Instance();

    void Initialize(ID3D11Device* device);
    void Shutdown();

    // Load texture from file (supports PNG, JPG, BMP via stb_image)
    Texture* LoadTexture(const std::string& path);

    // Get already loaded texture
    Texture* GetTexture(const std::string& path);

    // Unload texture
    void UnloadTexture(const std::string& path);

    // Clear all textures
    void ClearAll();

private:
    TextureManager() = default;
    ~TextureManager() = default;

    ID3D11Device* m_device = nullptr;
    std::unordered_map<std::string, Texture> m_textures;
};
