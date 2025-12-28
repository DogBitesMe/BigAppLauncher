#include "VideoPlayer.h"
#include <mferror.h>
#include <Shlwapi.h>  // For SHCreateMemStream (alternative) or use CreateStreamOnHGlobal

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Shlwapi.lib")

VideoPlayer::VideoPlayer() = default;

VideoPlayer::~VideoPlayer() {
    Shutdown();
}

bool VideoPlayer::Initialize(ID3D11Device* device, ID3D11DeviceContext* context) {
    m_device = device;
    m_context = context;

    // Initialize Media Foundation
    HRESULT hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) {
        return false;
    }

    m_isInitialized = true;
    return true;
}

void VideoPlayer::Shutdown() {
    Stop();

    m_sourceReader.Reset();
    m_byteStream.Reset();
    m_textureSRV.Reset();
    m_texture.Reset();

    if (m_isInitialized) {
        MFShutdown();
        m_isInitialized = false;
    }

    m_isLoaded = false;
    m_device = nullptr;
    m_context = nullptr;
}

bool VideoPlayer::LoadVideo(const std::wstring& path) {
    if (!m_isInitialized) {
        return false;
    }

    // Release previous resources
    m_sourceReader.Reset();
    m_byteStream.Reset();
    m_textureSRV.Reset();
    m_texture.Reset();
    m_isLoaded = false;

    // Create source reader attributes
    ComPtr<IMFAttributes> attributes;
    HRESULT hr = MFCreateAttributes(&attributes, 1);
    if (FAILED(hr)) return false;

    // Enable video processing (for format conversion)
    hr = attributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
    if (FAILED(hr)) return false;

    // Create source reader from file path
    hr = MFCreateSourceReaderFromURL(path.c_str(), attributes.Get(), &m_sourceReader);
    if (FAILED(hr)) {
        return false;
    }

    // Use shared initialization logic
    return InitializeFromSourceReader();
}

bool VideoPlayer::LoadVideoFromMemory(const void* data, size_t dataSize) {
    if (!m_isInitialized || !data || dataSize == 0) {
        return false;
    }

    // Release previous resources
    m_sourceReader.Reset();
    m_byteStream.Reset();
    m_textureSRV.Reset();
    m_texture.Reset();
    m_isLoaded = false;

    // Create IStream from memory using SHCreateMemStream
    // Note: SHCreateMemStream makes a copy of the data, so it's safe even if original data is freed
    ComPtr<IStream> memStream;
    memStream.Attach(SHCreateMemStream(
        static_cast<const BYTE*>(data),
        static_cast<UINT>(dataSize)
    ));
    if (!memStream) return false;

    // Create IMFByteStream from IStream
    HRESULT hr = MFCreateMFByteStreamOnStream(memStream.Get(), &m_byteStream);
    if (FAILED(hr)) return false;

    // Create source reader attributes
    ComPtr<IMFAttributes> attributes;
    hr = MFCreateAttributes(&attributes, 1);
    if (FAILED(hr)) return false;

    hr = attributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
    if (FAILED(hr)) return false;

    // Create source reader from byte stream
    hr = MFCreateSourceReaderFromByteStream(m_byteStream.Get(), attributes.Get(), &m_sourceReader);
    if (FAILED(hr)) return false;

    // Use shared initialization logic
    return InitializeFromSourceReader();
}

bool VideoPlayer::InitializeFromSourceReader() {
    // Configure output format to RGB32
    ComPtr<IMFMediaType> outputType;
    HRESULT hr = MFCreateMediaType(&outputType);
    if (FAILED(hr)) return false;

    hr = outputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    if (FAILED(hr)) return false;

    hr = outputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    if (FAILED(hr)) return false;

    hr = m_sourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, outputType.Get());
    if (FAILED(hr)) return false;

    // Get actual output format
    ComPtr<IMFMediaType> actualType;
    hr = m_sourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &actualType);
    if (FAILED(hr)) return false;

    // Get video dimensions
    UINT32 width = 0, height = 0;
    hr = MFGetAttributeSize(actualType.Get(), MF_MT_FRAME_SIZE, &width, &height);
    if (FAILED(hr)) return false;

    m_width = width;
    m_height = height;

    // Get stride (can be negative for bottom-up images)
    UINT32 strideVal = 0;
    hr = actualType->GetUINT32(MF_MT_DEFAULT_STRIDE, &strideVal);
    if (SUCCEEDED(hr)) {
        m_stride = static_cast<INT32>(strideVal);
    } else {
        // Try to get it from MFGetStrideForBitmapInfoHeader
        LONG stride = 0;
        hr = MFGetStrideForBitmapInfoHeader(MFVideoFormat_RGB32.Data1, width, &stride);
        if (SUCCEEDED(hr)) {
            m_stride = stride;
        } else {
            // Fallback: calculate stride (positive = top-down)
            m_stride = width * 4;
        }
    }

    // Get frame rate
    UINT32 numerator = 0, denominator = 1;
    hr = MFGetAttributeRatio(actualType.Get(), MF_MT_FRAME_RATE, &numerator, &denominator);
    if (SUCCEEDED(hr) && numerator > 0) {
        m_frameRate = static_cast<float>(numerator) / static_cast<float>(denominator);
    } else {
        m_frameRate = 30.0f;
    }
    m_frameTime = 1.0f / m_frameRate;

    // Get duration
    PROPVARIANT var;
    PropVariantInit(&var);
    hr = m_sourceReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var);
    if (SUCCEEDED(hr)) {
        m_duration = var.hVal.QuadPart;
        PropVariantClear(&var);
    }

    // Create texture
    if (!CreateTexture(m_width, m_height)) {
        return false;
    }

    // Decode first frame
    if (!DecodeFrame()) {
        return false;
    }

    m_isLoaded = true;
    m_accumTime = 0.0f;
    return true;
}

bool VideoPlayer::CreateTexture(int width, int height) {
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // RGB32 from MF is actually BGRA
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = m_device->CreateTexture2D(&desc, nullptr, &m_texture);
    if (FAILED(hr)) return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = m_device->CreateShaderResourceView(m_texture.Get(), &srvDesc, &m_textureSRV);
    return SUCCEEDED(hr);
}

bool VideoPlayer::DecodeFrame() {
    if (!m_sourceReader) return false;

    DWORD streamIndex = 0;
    DWORD flags = 0;
    LONGLONG timestamp = 0;
    ComPtr<IMFSample> sample;

    HRESULT hr = m_sourceReader->ReadSample(
        MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        0,
        &streamIndex,
        &flags,
        &timestamp,
        &sample
    );

    if (FAILED(hr)) return false;

    // Check for end of stream
    if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
        if (m_loop) {
            // Seek to beginning
            PROPVARIANT var;
            PropVariantInit(&var);
            var.vt = VT_I8;
            var.hVal.QuadPart = 0;
            m_sourceReader->SetCurrentPosition(GUID_NULL, var);
            PropVariantClear(&var);
            return DecodeFrame(); // Try again from start
        }
        m_isPlaying = false;
        return false;
    }

    if (!sample) return false;

    // Get buffer from sample
    ComPtr<IMFMediaBuffer> buffer;
    hr = sample->ConvertToContiguousBuffer(&buffer);
    if (FAILED(hr)) return false;

    CopyFrameToTexture(buffer.Get());
    m_frameCount++;
    return true;
}

void VideoPlayer::CopyFrameToTexture(IMFMediaBuffer* buffer) {
    if (!buffer || !m_texture || !m_context) return;

    BYTE* srcData = nullptr;
    DWORD maxLength = 0, currentLength = 0;

    HRESULT hr = buffer->Lock(&srcData, &maxLength, &currentLength);
    if (FAILED(hr)) return;

    // Map texture
    D3D11_MAPPED_SUBRESOURCE mapped;
    hr = m_context->Map(m_texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (SUCCEEDED(hr)) {
        BYTE* dst = static_cast<BYTE*>(mapped.pData);
        UINT32 dstStride = mapped.RowPitch;

        // Handle both top-down (positive stride) and bottom-up (negative stride) images
        INT32 absStride = (m_stride < 0) ? -m_stride : m_stride;

        for (int y = 0; y < m_height; y++) {
            // Calculate source row based on stride sign
            int srcY = (m_stride < 0) ? (m_height - 1 - y) : y;
            BYTE* srcRow = srcData + srcY * absStride;
            BYTE* dstRow = dst + y * dstStride;

            // Copy pixel by pixel, ensuring alpha is set to 255
            // RGB32 format: B, G, R, A (little endian on x86)
            for (int x = 0; x < m_width; x++) {
                dstRow[x * 4 + 0] = srcRow[x * 4 + 0]; // B
                dstRow[x * 4 + 1] = srcRow[x * 4 + 1]; // G
                dstRow[x * 4 + 2] = srcRow[x * 4 + 2]; // R
                dstRow[x * 4 + 3] = 255;               // A - Force opaque
            }
        }

        m_context->Unmap(m_texture.Get(), 0);
    }

    buffer->Unlock();
}

void VideoPlayer::Update(float deltaTime) {
    if (!m_isLoaded || !m_isPlaying) return;

    m_accumTime += deltaTime;

    // Decode new frame based on frame rate
    if (m_accumTime >= m_frameTime) {
        m_accumTime -= m_frameTime;
        DecodeFrame();
    }
}

void VideoPlayer::Play() {
    if (m_isLoaded) {
        m_isPlaying = true;
    }
}

void VideoPlayer::Pause() {
    m_isPlaying = false;
}

void VideoPlayer::Stop() {
    m_isPlaying = false;
    m_accumTime = 0.0f;

    // Seek to beginning
    if (m_sourceReader) {
        PROPVARIANT var;
        PropVariantInit(&var);
        var.vt = VT_I8;
        var.hVal.QuadPart = 0;
        m_sourceReader->SetCurrentPosition(GUID_NULL, var);
        PropVariantClear(&var);
    }
}
