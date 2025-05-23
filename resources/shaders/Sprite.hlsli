struct VSOutput
{
    float32_t4 mSVPosition : SV_POSITION0;
    float32_t2 mUV : TEXCOORD0;
};

struct Constant
{
    float32_t4x4 mWVPMat;
    float32_t4x4 mUVMat;
    float32_t4 mColor;
};

ConstantBuffer<Constant> gConstant : register(b0);
