// Gaussian blur pixel shader (two-pass: horizontal then vertical)

Texture2D inputTexture : register(t0);
SamplerState linearSampler : register(s0);

cbuffer BlurParams : register(b0) {
    float2 texelSize;    // 1.0 / textureSize
    float2 direction;    // (1,0) for horizontal, (0,1) for vertical
    float blurRadius;    // Number of samples
    float padding;
};

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// Gaussian weights for 9-tap blur
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
