#include "Primitive.hlsli"

struct VSInput
{
    float32_t4 mPosition : POSITION0;
    float32_t4 mColor : COLOR0;
};

struct ViewProjection
{
    float32_t4x4 mMat;
};

ConstantBuffer<ViewProjection> gViewProjection : register(b0);

VSOutput main(VSInput input)
{
    VSOutput output;
    output.mSVPosition = mul(input.mPosition, gViewProjection.mMat);
    output.mColor = input.mColor;
    return output;
}
