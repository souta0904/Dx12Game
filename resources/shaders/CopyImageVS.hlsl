#include "CopyImage.hlsli"

static const uint32_t kVertNum = 3;
static const float32_t4 kPositions[kVertNum] =
{
    float32_t4(-1.0f, 1.0f, 0.0f, 1.0f),
    float32_t4(3.0f, 1.0f, 0.0f, 1.0f),
    float32_t4(-1.0f, -3.0f, 0.0f, 1.0f)
};
static const float32_t2 kUVs[kVertNum] =
{
    float32_t2(0.0f, 0.0f),
    float32_t2(2.0f, 0.0f),
    float32_t2(0.0f, 2.0f)
};

VSOutput main(uint32_t vertexID : SV_VertexID)
{
    VSOutput output;
    output.mSVPosition = kPositions[vertexID];
    output.mUV = kUVs[vertexID];
    return output;
}
