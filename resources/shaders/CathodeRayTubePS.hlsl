#include "CopyImage.hlsli"

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PSOutput
{
    float32_t4 mColor : SV_TARGET0;
};

// CRT用定数
struct Constant
{
    float32_t mTime;
    float32_t mZoom;
    float32_t mDistortion;
    float32_t mNoise;
    float32_t mNoiseScaleX;
    float32_t mNoiseScaleY;
    float32_t mNoiseSpeed;
    float32_t2 mRGBShift;
};
ConstantBuffer<Constant> gConstant : register(b0);

// 歪み関数
float32_t2 Distortion(float32_t2 uv)
{
    float32_t2 centeredUV = (uv - 0.5) * gConstant.mZoom + 0.5;
    centeredUV = centeredUV * 2.0 - 1.0;
    float32_t radiusSq = dot(centeredUV, centeredUV);
    float32_t f = 1.0 + gConstant.mDistortion * radiusSq;
    return (centeredUV * f) * 0.5 + 0.5;
}

PSOutput main(VSOutput input)
{
    PSOutput output;
    // 歪み
    float32_t2 uv = Distortion(input.mUV);
    // ノイズ
    float32_t noise =
        sin(uv.y * gConstant.mNoiseScaleY) * gConstant.mNoise +
        (sin(uv.x * gConstant.mNoiseScaleY + gConstant.mTime * gConstant.mNoiseSpeed) *
         sin(uv.y * gConstant.mNoiseScaleY + gConstant.mTime * gConstant.mNoiseSpeed)) *
        (gConstant.mNoise * 0.2f);
    // RGBずらし
    float32_t2 uvR = uv + gConstant.mRGBShift.x;
    float32_t2 uvG = uv;
    float32_t2 uvB = uv - gConstant.mRGBShift.y;
    
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
