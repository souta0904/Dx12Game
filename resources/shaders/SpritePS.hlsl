#include "Sprite.hlsli"

struct PSOutput
{
    float32_t4 mColor : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(VSOutput input)
{
    PSOutput output;
    float32_t4 transformedUV = mul(float32_t4(input.mUV, 0.0f, 1.0f), gConstant.mUVMat);
    float32_t4 texColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.mColor = texColor * gConstant.mColor;
    return output;
}
