#include "Model.hlsli"

struct VSInput
{
    float32_t4 mPosition : POSITION0;
    float32_t3 mNormal : NORMAL0;
    float32_t2 mUV : TEXCOORD0;
    float32_t4 mWeights : WEIGHT0;
    int32_t4 mBoneIndices : BONE_INDEX0;
};

struct TransformationMatrix
{
    float32_t4x4 mWorldMat;
    float32_t4x4 mWVPMat;
    float32_t4x4 mWorldInverseTransposeMat;
};

struct SkinnedVertex
{
    float32_t4 mPosition;
    float32_t3 mNormal;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
StructuredBuffer<float32_t4x4> gPalette : register(t0);

SkinnedVertex CalcSkinning(VSInput input)
{
    SkinnedVertex output;
    output.mPosition = mul(input.mPosition, gPalette[input.mBoneIndices.x]) * input.mWeights.x;
    output.mPosition += mul(input.mPosition, gPalette[input.mBoneIndices.y]) * input.mWeights.y;
    output.mPosition += mul(input.mPosition, gPalette[input.mBoneIndices.z]) * input.mWeights.z;
    output.mPosition += mul(input.mPosition, gPalette[input.mBoneIndices.w]) * input.mWeights.w;
    output.mNormal = mul(input.mNormal, (float32_t3x3) gPalette[input.mBoneIndices.x]) * input.mWeights.x;
    output.mNormal += mul(input.mNormal, (float32_t3x3) gPalette[input.mBoneIndices.y]) * input.mWeights.y;
    output.mNormal += mul(input.mNormal, (float32_t3x3) gPalette[input.mBoneIndices.z]) * input.mWeights.z;
    output.mNormal += mul(input.mNormal, (float32_t3x3) gPalette[input.mBoneIndices.w]) * input.mWeights.w;
    return output;
}

VSOutput main(VSInput input)
{
    VSOutput output;
    SkinnedVertex skinned = CalcSkinning(input);
    output.mWPosition = mul(skinned.mPosition, gTransformationMatrix.mWorldMat).xyz;
    output.mSVPosition = mul(skinned.mPosition, gTransformationMatrix.mWVPMat);
    output.mNormal = normalize(mul(skinned.mNormal, (float32_t3x3) gTransformationMatrix.mWorldInverseTransposeMat));
    output.mUV = input.mUV;
    return output;
}
