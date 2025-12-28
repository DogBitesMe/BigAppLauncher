#pragma once

#include <d3d11.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl/client.h>
#include <string>

using Microsoft::WRL::ComPtr;

class VideoPlayer {
public:
    VideoPlayer();
    ~VideoPlayer();

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    void Shutdown();

    bool LoadVideo(const std::wstring& path);
    bool LoadVideoFromMemory(const void* data, size_t dataSize);
    void Update(float deltaTime);

    void Play();
    void Pause();
    void Stop();
    void SetLoop(bool loop) { m_loop = loop; }

    bool IsPlaying() const { return m_isPlaying; }
    bool IsLoaded() const { return m_isLoaded; }

    ID3D11ShaderResourceView* GetTextureSRV() const { return m_textureSRV.Get(); }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    INT32 GetStride() const { return m_stride; }
    int GetFrameCount() const { return m_frameCount; }

private:
    bool CreateTexture(int width, int height);
    bool DecodeFrame();
    void CopyFrameToTexture(IMFMediaBuffer* buffer);
    bool InitializeFromSourceReader();  // Shared initialization after source reader creation

    // DirectX
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    ComPtr<ID3D11Texture2D> m_texture;
    ComPtr<ID3D11ShaderResourceView> m_textureSRV;

    // Media Foundation
    ComPtr<IMFSourceReader> m_sourceReader;
    ComPtr<IMFByteStream> m_byteStream;  // For memory-based loading

    // Video info
    int m_width = 0;
    int m_height = 0;
    INT32 m_stride = 0;  // Can be negative for bottom-up images
    LONGLONG m_duration = 0;
    float m_frameRate = 30.0f;
    float m_frameTime = 0.0f;
    float m_accumTime = 0.0f;

    // State
    bool m_isInitialized = false;
    bool m_isLoaded = false;
    bool m_isPlaying = false;
    bool m_loop = true;
    int m_frameCount = 0;
};
