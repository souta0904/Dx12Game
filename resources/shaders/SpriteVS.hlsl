#include "Sprite.hlsli"

struct VSInput
{
    float32_t4 mPosition : POSITION0;
    float32_t2 mUV : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.mSVPosition = mul(input.mPosition, gConstant.mWVPMat);
    output.mUV = input.mUV;
    return output;
}
