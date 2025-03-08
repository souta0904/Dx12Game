#pragma once
#include "ModelCamera.h"
#include "directX/PipelineState.h"
#include "PSOFlags.h"
#include <memory>
#include <string>
#include <unordered_map>

struct LightCommon {
	uint32_t mDirectionalLightNum;
	uint32_t mPointLightNum;
	uint32_t mSpotLightNum;
};

struct DirectionalLight {
	Vector3 mColor;
	float mIntensity;
	Vector3 mDirection;
};

struct PointLight {
	Vector3 mColor;
	float mIntensity;
	Vector3 mPosition;
	float mRadius;
	float mDecay;
};

struct SpotLight {
	Vector3 mColor;
	float mIntensity;
	Vector3 mDirection;
	Vector3 mPosition;
	float mRadius;
	float mDecay;
	float mInner;
	float mOuter;
};

class ModelBase {
public:
	friend class Model;

	static ModelBase& GetInstance() {
		static ModelBase instance;
		return instance;
	}
	ModelBase(const ModelBase&) = delete;
	ModelBase& operator=(const ModelBase&) = delete;
	ModelBase(ModelBase&&) = delete;
	ModelBase& operator=(ModelBase&&) = delete;

	enum class RootParamIdx {
		kTransformationMat,
		kMaterial,
		kTexture,
		kCamera,
		kLightCommon,
		kDirectionalLight,
		kPointLight,
		kSpotLight,
		kSkin,
		kMax
	};

	void Initialize();
	void Terminate();
	void Prepare();
	void SetPipelineState(PSOFlags flags);
	ModelCamera* GetDefaultCamera() const { return mDefaultCamera.get(); }

	void AddDirectionalLight(DirectionalLight* light);
	void AddPointLight(PointLight* light);
	void AddSpotLight(SpotLight* light);
	void RemoveDirectionalLight(DirectionalLight* light);
	void RemovePointLight(PointLight* light);
	void RemoveSpotLight(SpotLight* light);

private:
	ModelBase() = default;
	~ModelBase() = default;
	PipelineState* GetPipelineState(PSOFlags flags);
	PipelineState* CreatePipelineState(PSOFlags flags);

private:
	std::unique_ptr<RootSignature> mRootSignature;
	std::unordered_map<std::string, D3D12_INPUT_ELEMENT_DESC> mInputLayouts;
	std::unordered_map<PSOFlags, std::unique_ptr<PipelineState>> mPipelineStates;
	std::unique_ptr<ModelCamera> mDefaultCamera;

	static const uint32_t kMaxDirectionalLightNum = 256;
	static const uint32_t kMaxPointLightNum = 256;
	static const uint32_t kMaxSpotLightNum = 256;
	std::unique_ptr<ConstantBuffer> mLightCommonBuff;
	std::unique_ptr<StructuredBuffer> mDirectionalLightBuff;
	std::unique_ptr<StructuredBuffer> mPointLightBuff;
	std::unique_ptr<StructuredBuffer> mSpotLightBuff;
	std::vector<DirectionalLight*> mDirectionalLights;
	std::vector<PointLight*> mPointLights;
	std::vector<SpotLight*> mSpotLights;
};
