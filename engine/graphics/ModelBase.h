#pragma once
#include "ModelCamera.h"
#include "directX/PipelineState.h"
#include "Light.h"
#include "PSOFlags.h"
#include <memory>
#include <string>
#include <unordered_map>

// モデル基盤
class ModelBase {
public:
	friend class ModelInstance;
	// シングルトンを実装
	static ModelBase& GetInstance() {
		static ModelBase instance;
		return instance;
	}
	ModelBase(const ModelBase&) = delete;
	ModelBase& operator=(const ModelBase&) = delete;
	ModelBase(ModelBase&&) = delete;
	ModelBase& operator=(ModelBase&&) = delete;
private:
	ModelBase() = default;
	~ModelBase() = default;

public:
	// ルートパラメータ番号
	enum class RootParamNum {
		kTransformationMat,	// 変換行列
		kMaterial,			// マテリアル
		kTexture,			// テクスチャ
		kCamera,			// カメラ座標
		kLightCommon,		// ライト共通
		kDirectionalLight,	// 平行光源
		kPointLight,		// 点光源
		kSpotLight,			// スポットライト
		kMatPalette,		// 行列パレット
		kMax
	};

	void Initialize();
	void Terminate();
	// モデル描画準備
	void Prepare();
	// PSOフラグでパイプラインステートをセット
	void SetPipelineState(PSOFlags flags);

	ModelCamera* GetDefaultCamera() const { return mDefaultCamera.get(); }

	// ライト関連(追加と削除)
	void AddDirectionalLight(DirectionalLight* light);
	void AddPointLight(PointLight* light);
	void AddSpotLight(SpotLight* light);
	void RemoveDirectionalLight(DirectionalLight* light);
	void RemovePointLight(PointLight* light);
	void RemoveSpotLight(SpotLight* light);

private:
	PipelineState* GetPipelineState(PSOFlags flags);// 取得
	PipelineState* CreatePipelineState(PSOFlags flags);// 作成
	// ライト更新
	void UpdateLight();

private:
	std::unique_ptr<RootSignature> mRootSignature;
	std::unordered_map<PSOFlags, std::unique_ptr<PipelineState>> mPipelineStates;
	std::unique_ptr<ModelCamera> mDefaultCamera;
	// セマンティック名と頂点レイアウトの要素のマップ
	std::unordered_map<std::string, D3D12_INPUT_ELEMENT_DESC> mInputLayoutMap;

	// ライト関連
	static const uint32_t kMaxDirectionalLightNum = 8;
	static const uint32_t kMaxPointLightNum = 8;
	static const uint32_t kMaxSpotLightNum = 8;
	std::unique_ptr<ConstantBuffer> mLightCommonBuff;
	std::unique_ptr<StructuredBuffer> mDirectionalLightBuff;
	std::unique_ptr<StructuredBuffer> mPointLightBuff;
	std::unique_ptr<StructuredBuffer> mSpotLightBuff;
	std::vector<DirectionalLight*> mDirectionalLights;
	std::vector<PointLight*> mPointLights;
	std::vector<SpotLight*> mSpotLights;
};
