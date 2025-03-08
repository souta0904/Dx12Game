#include "CopyImage.hlsli"

static const uint32_t kVertexCount = 3;

static const float32_t4 kPositions[kVertexCount] =
{
    float32_t4(-1.0f, 1.0f, 0.0f, 1.0f),
    float32_t4(3.0f, 1.0f, 0.0f, 1.0f),
    float32_t4(-1.0f, -3.0f, 0.0f, 1.0f)
};

static const float32_t2 kUVs[kVertexCount] =
{
    float32_t2(0.0f, 0.0f),
    float32_t2(2.0f, 0.0f),
    float32_t2(0.0f, 2.0f)
};

VSOutput main(uint32_t vertexId : SV_VertexID)
{
    VSOutput output;
    output.mPosition = kPositions[vertexId];
    output.mUV = kUVs[vertexId];
    return output;
}
