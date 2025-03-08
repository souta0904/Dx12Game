#include "Model.hlsli"

struct VSInput
{
    float32_t4 mPosition : POSITION0;
    float32_t3 mNormal : NORMAL0;
    float32_t2 mUV : TEXCOORD0;
};

struct TransformationMatrix
{
    float32_t4x4 mWorldMat;
    float32_t4x4 mWVPMat;
    float32_t4x4 mWorldInverseTransposeMat;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VSOutput main(VSInput input)
{
    VSOutput output;
    output.mWPosition = mul(input.mPosition, gTransformationMatrix.mWorldMat).xyz;
    output.mSVPosition = mul(input.mPosition, gTransformationMatrix.mWVPMat);
    output.mNormal = normalize(mul(input.mNormal, (float32_t3x3) gTransformationMatrix.mWorldInverseTransposeMat));
    output.mUV = input.mUV;
    return output;
}
