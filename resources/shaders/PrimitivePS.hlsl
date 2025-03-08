#include "Primitive.hlsli"

struct PSOutput
{
    float32_t4 mColor : SV_TARGET0;
};

PSOutput main(VSOutput input)
{
    PSOutput output;
    output.mColor = input.mColor;
    return output;
}
