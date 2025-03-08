#include "Model.hlsli"

struct PSOutput
{
    float32_t4 mColor : SV_TARGET0;
};

enum
class SpecularType
{
    kNone,
	kPhong,
	kBlinnPhong
};

struct Material
{
    float32_t4 mColor;
    float32_t4x4 mUVMat;
    int32_t mIsLighting;
    SpecularType mSpecularType;
    float32_t mShininess;
};

struct Camera
{
    float32_t3 mPosition;
};

struct LightCommon
{
    uint32_t mDirectionalLightNum;
    uint32_t mPointLightNum;
    uint32_t mSpotLightNum;
};

struct DirectionalLight
{
    float32_t3 mColor;
    float32_t mIntensity;
    float32_t3 mDirection;
};

struct PointLight
{
    float32_t3 mColor;
    float32_t mIntensity;
    float32_t3 mPosition;
    float32_t mRadius;
    float32_t mDecay;
};

struct SpotLight
{
    float32_t3 mColor;
    float32_t mIntensity;
    float32_t3 mDirection;
    float32_t3 mPosition;
    float32_t mRadius;
    float32_t mDecay;
    float32_t mInnerCos;
    float32_t mOuterCos;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);
ConstantBuffer<LightCommon> gLightCommon : register(b2);
Texture2D<float32_t4> gTexture : register(t0);
StructuredBuffer<DirectionalLight> gDirectionalLights : register(t1);
StructuredBuffer<PointLight> gPointLights : register(t2);
StructuredBuffer<SpotLight> gSpotLights : register(t3);
SamplerState gSampler : register(s0);

PSOutput main(VSOutput input)
{
    PSOutput output;
    
#ifdef NO_TEXTURE
    float32_t4 surfaceColor = gMaterial.mColor;
#else
    float32_t4 transformedUV = mul(float32_t4(input.mUV, 0.0f, 1.0f), gMaterial.mUVMat);
    float32_t4 texColor = gTexture.Sample(gSampler, transformedUV.xy);
    if (texColor.a <= 0.0f)
    {
        discard;
    }
    float32_t4 surfaceColor = texColor * gMaterial.mColor;
#endif
    
    if (gMaterial.mIsLighting)
    {
        float32_t3 toEye = normalize(gCamera.mPosition - input.mWPosition);
        float32_t3 shadeColor = float32_t3(0.0f, 0.0f, 0.0f);
        float32_t3 specularColor = float32_t3(1.0f, 1.0f, 1.0f);
        for (uint32_t i = 0; i < gLightCommon.mDirectionalLightNum; ++i)
        {
            float32_t3 lightColor = gDirectionalLights[i].mColor * gDirectionalLights[i].mIntensity;
            float32_t NdotL = dot(input.mNormal, -gDirectionalLights[i].mDirection);
            float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            shadeColor += surfaceColor.rgb * lightColor * cos;
            if (gMaterial.mSpecularType == SpecularType::kPhong)
            {
                float32_t3 R = reflect(gDirectionalLights[i].mDirection, input.mNormal);
                float32_t RdotE = dot(R, toEye);
                float32_t specularPow = pow(saturate(RdotE), gMaterial.mShininess);
                shadeColor += lightColor * specularPow * specularColor;
            }
            else if (gMaterial.mSpecularType == SpecularType::kBlinnPhong)
            {
                float32_t3 H = normalize(-gDirectionalLights[i].mDirection + toEye);
                float32_t NdotH = dot(input.mNormal, H);
                float32_t specularPow = pow(saturate(NdotH), gMaterial.mShininess);
                shadeColor += lightColor * specularPow * specularColor;
            }
        }
        for (i = 0; i < gLightCommon.mPointLightNum; ++i)
        {
            float32_t3 lightColor = gPointLights[i].mColor * gPointLights[i].mIntensity;
            float32_t3 direction = normalize(input.mWPosition - gPointLights[i].mPosition);
            float32_t NdotL = dot(input.mNormal, -direction);
            float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            float32_t distance = length(gPointLights[i].mPosition - input.mWPosition);
            float32_t factor = 0.0f;
            if (gPointLights[i].mRadius > 0.0f)
            {
                factor = pow(saturate(-distance / gPointLights[i].mRadius + 1.0f), gPointLights[i].mDecay);
            }
            shadeColor += surfaceColor.rgb * lightColor * cos * factor;
            if (gMaterial.mSpecularType == SpecularType::kPhong)
            {
                float32_t3 R = reflect(direction, input.mNormal);
                float32_t RdotE = dot(R, toEye);
                float32_t specularPow = pow(saturate(RdotE), gMaterial.mShininess);
                shadeColor += lightColor * specularPow * specularColor * factor;
            }
            else if (gMaterial.mSpecularType == SpecularType::kBlinnPhong)
            {
                float32_t3 H = normalize(-direction + toEye);
                float32_t NdotH = dot(input.mNormal, H);
                float32_t specularPow = pow(saturate(NdotH), gMaterial.mShininess);
                shadeColor += lightColor * specularPow * specularColor * factor;
            }
        }
        for (i = 0; i < gLightCommon.mSpotLightNum; ++i)
        {
            float32_t3 lightColor = gSpotLights[i].mColor * gSpotLights[i].mIntensity;
            float32_t3 direction = normalize(input.mWPosition - gSpotLights[i].mPosition);
            float32_t NdotL = dot(input.mNormal, -direction);
            float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            float32_t distance = length(gSpotLights[i].mPosition - input.mWPosition);
            float32_t factor = 0.0f;
            if (gSpotLights[i].mRadius > 0.0f)
            {
                factor = pow(saturate(-distance / gSpotLights[i].mRadius + 1.0f), gSpotLights[i].mDecay);
            }
            float32_t dirCos = dot(direction, gSpotLights[i].mDirection);
            factor *= saturate((dirCos - gSpotLights[i].mOuterCos) / (gSpotLights[i].mInnerCos - gSpotLights[i].mOuterCos));
            shadeColor += surfaceColor.rgb * lightColor * cos * factor;
            if (gMaterial.mSpecularType == SpecularType::kPhong)
            {
                float32_t3 R = reflect(direction, input.mNormal);
                float32_t RdotE = dot(R, toEye);
                float32_t specularPow = pow(saturate(RdotE), gMaterial.mShininess);
                shadeColor += lightColor * specularPow * specularColor * factor;
            }
            else if (gMaterial.mSpecularType == SpecularType::kBlinnPhong)
            {
                float32_t3 H = normalize(-direction + toEye);
                float32_t NdotH = dot(input.mNormal, H);
                float32_t specularPow = pow(saturate(NdotH), gMaterial.mShininess);
                shadeColor += lightColor * specularPow * specularColor * factor;
            }
        }
        output.mColor.rgb = shadeColor;
        output.mColor.a = surfaceColor.a;
    }
    else
    {
        output.mColor = surfaceColor;
    }
    return output;
}
