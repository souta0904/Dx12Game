#include "CopyImage.hlsli"

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PSOutput
{
    float32_t4 mColor : SV_TARGET0;
};

struct Time
{
    float32_t mTime;
};
ConstantBuffer<Time> gTime : register(b0);

// k: 歪み定数
float32_t2 Distortion(float32_t2 uv, float32_t k)
{
    float32_t2 zoom = 0.95f;
    float32_t2 centeredUV = (uv - 0.5) * zoom + 0.5;
    centeredUV = centeredUV * 2.0 - 1.0;
    float32_t radiusSq = dot(centeredUV, centeredUV);
    float32_t f = 1.0 + k * radiusSq;
    return (centeredUV * f) * 0.5 + 0.5;
}

PSOutput main(VSOutput input)
{
    PSOutput output;
    
    float32_t2 uv = Distortion(input.mUV, 0.2f);
    
    float32_t noise =
        sin(uv.y * 800.0f) * 0.05f +
        (sin(uv.x * 150.0f + gTime.mTime * 50.0f) * sin(uv.y * 150.0f + gTime.mTime * 50.0f)) * 0.01f;
    
    float32_t2 uvR = uv + float32_t2(0.00225f, 0.004f);
    float32_t2 uvG = uv;
    float32_t2 uvB = uv + float32_t2(0.00225f, 0.004f);
    
    if (uvR.x < 0 || uvR.x > 1 || uvR.y < 0 || uvR.y > 1 ||
        uvG.x < 0 || uvG.x > 1 || uvG.y < 0 || uvG.y > 1 ||
        uvB.x < 0 || uvB.x > 1 || uvB.y < 0 || uvB.y > 1)
    {
        output.mColor = float32_t4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        output.mColor.r = gTexture.Sample(gSampler, uvR).r + noise;
        output.mColor.g = gTexture.Sample(gSampler, uvG).g + noise;
        output.mColor.b = gTexture.Sample(gSampler, uvB).b + noise;
        output.mColor.a = 1.0;
    }
    
    return output;
}
