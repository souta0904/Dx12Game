#include "CopyImage.hlsli"

struct PSOutput
{
    float32_t4 mColor : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PSOutput main(VSOutput input)
{
    PSOutput output;
    output.mColor = gTexture.Sample(gSampler, input.mUV);
    return output;
}
