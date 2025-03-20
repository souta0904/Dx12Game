#include "ModelBase.h"
#include "directX/DirectXBase.h"
#include "directX/DirectXCommon.h"
#include "ResourceMgr.h"

void ModelBase::Initialize() {
	// ルートシグネチャ作成
	mRootSignature = std::make_unique<RootSignature>(static_cast<uint32_t>(RootParamNum::kMax), 1);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kTransformationMat)).InitCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kMaterial)).InitCBV(0, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kTexture)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kTexture)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kCamera)).InitCBV(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kLightCommon)).InitCBV(2, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kDirectionalLight)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kDirectionalLight)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kPointLight)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kPointLight)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kSpotLight)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kSpotLight)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kMatPalette)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_VERTEX);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kMatPalette)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mRootSignature->GetSampler(0) = DirectXCommon::gSamplerLinearClamp;
	mRootSignature->Create();

	// 頂点レイアウトを定義
	D3D12_INPUT_ELEMENT_DESC desc = {};
	desc.SemanticName = "POSITION";
	desc.SemanticIndex = 0;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	mInputLayoutMap.emplace(desc.SemanticName, desc);
	desc.SemanticName = "NORMAL";
	desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	mInputLayoutMap.emplace(desc.SemanticName, desc);
	desc.SemanticName = "TEXCOORD";
	desc.Format = DXGI_FORMAT_R32G32_FLOAT;
	mInputLayoutMap.emplace(desc.SemanticName, desc);
	desc.SemanticName = "WEIGHT";
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	mInputLayoutMap.emplace(desc.SemanticName, desc);
	desc.SemanticName = "BONE_INDEX";
	desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
	mInputLayoutMap.emplace(desc.SemanticName, desc);

	// パイプラインステートの作成
	CreatePipelineState(PSOFlags::kNone);// テクスチャ、スキンなし
	CreatePipelineState(PSOFlags::kTexture);// テクスチャあり
	CreatePipelineState(PSOFlags::kSkin);// スキンあり
	CreatePipelineState(PSOFlags::kTexture | PSOFlags::kSkin);// テクスチャ、スキンあり

	// ライト関連を初期化
	mLightCommonBuff = std::make_unique<ConstantBuffer>();
	mLightCommonBuff->Create(sizeof(LightCommon));
	mDirectionalLightBuff = std::make_unique<StructuredBuffer>();
	mDirectionalLightBuff->Create(kMaxDirectionalLightNum, sizeof(DirectionalLight));
	mPointLightBuff = std::make_unique<StructuredBuffer>();
	mPointLightBuff->Create(kMaxPointLightNum, sizeof(PointLight));
	mSpotLightBuff = std::make_unique<StructuredBuffer>();
	mSpotLightBuff->Create(kMaxSpotLightNum, sizeof(SpotLight));

	// デフォルトのカメラ
	mDefaultCamera = std::make_unique<ModelCamera>();
	mDefaultCamera->Create();
	mDefaultCamera->mTranslate = Vector3(0.0f, 1.0f, -3.0f);
}

void ModelBase::Terminate() {

}

// モデル描画準備
void ModelBase::Prepare() {
	// デフォルトカメラの更新
	mDefaultCamera->UpdateMatrix();
	// ライト関連の更新
	UpdateLight();

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = DirectXBase::GetInstance().GetCmdList();
	mRootSignature->Set(cmdList);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ライト関連をセット
	mLightCommonBuff->Set(cmdList, static_cast<uint32_t>(RootParamNum::kLightCommon));
	mDirectionalLightBuff->Set(cmdList, static_cast<uint32_t>(RootParamNum::kDirectionalLight));
	mPointLightBuff->Set(cmdList, static_cast<uint32_t>(RootParamNum::kPointLight));
	mSpotLightBuff->Set(cmdList, static_cast<uint32_t>(RootParamNum::kSpotLight));
}

// PSOフラグでパイプラインステートをセット
void ModelBase::SetPipelineState(PSOFlags flags) {
	PipelineState* ps = GetPipelineState(flags);
	ps->Set(DirectXBase::GetInstance().GetCmdList());
}

void ModelBase::AddDirectionalLight(DirectionalLight* light) {
	mDirectionalLights.emplace_back(light);
}

void ModelBase::AddPointLight(PointLight* light) {
	mPointLights.emplace_back(light);
}

void ModelBase::AddSpotLight(SpotLight* light) {
	mSpotLights.emplace_back(light);
}

void ModelBase::RemoveDirectionalLight(DirectionalLight* light) {
	auto it = std::find(mDirectionalLights.begin(), mDirectionalLights.end(), light);
	if (it != mDirectionalLights.end()) {
		mDirectionalLights.erase(it);
	}
}

void ModelBase::RemovePointLight(PointLight* light) {
	auto it = std::find(mPointLights.begin(), mPointLights.end(), light);
	if (it != mPointLights.end()) {
		mPointLights.erase(it);
	}
}

void ModelBase::RemoveSpotLight(SpotLight* light) {
	auto it = std::find(mSpotLights.begin(), mSpotLights.end(), light);
	if (it != mSpotLights.end()) {
		mSpotLights.erase(it);
	}
}

// パイプラインステートを取得
PipelineState* ModelBase::GetPipelineState(PSOFlags flags) {
	auto it = mPipelineStates.find(flags);
	if (it != mPipelineStates.end()) {
		return it->second.get();
	} else {
		// なかったら作成する
		return CreatePipelineState(flags);
	}
}

// パイプラインステートを作成
PipelineState* ModelBase::CreatePipelineState(PSOFlags flags) {
	ResourceMgr& resourceMgr = ResourceMgr::GetInstance();

	PSOInit init;
	init.mRootSignature = mRootSignature.get();
	// 頂点シェーダ
	if ((flags & PSOFlags::kSkin) == PSOFlags::kSkin) {
		init.mVertexShader = resourceMgr.GetShader("resources/shaders/SkinModelVS.hlsl", "vs_6_0");
	} else {
		init.mVertexShader = resourceMgr.GetShader("resources/shaders/ModelVS.hlsl", "vs_6_0");
	}
	// ピクセルシェーダ
	if ((flags & PSOFlags::kTexture) == PSOFlags::kTexture) {
		init.mPixelShader = resourceMgr.GetShader("resources/shaders/ModelPS.hlsl", "ps_6_0");
	} else {
		init.mPixelShader = resourceMgr.GetShader("resources/shaders/NoTextureModelPS.hlsl", "ps_6_0");
	}
	// ブレンド
	if ((flags & PSOFlags::kNormalBlend) == PSOFlags::kNormalBlend) {
		init.mBlendDesc = DirectXCommon::gBlendNormal;
	} else {
		init.mBlendDesc = DirectXCommon::gBlendNone;
	}
	// ラスタライザ
	D3D12_RASTERIZER_DESC rasterizerDesc = DirectXCommon::gRasterizerDefault;
	if ((flags & PSOFlags::kCurrNone) == PSOFlags::kCurrNone) {
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	}
	if ((flags & PSOFlags::kWireframe) == PSOFlags::kWireframe) {
		rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}
	init.mRasterizerDesc = rasterizerDesc;
	// 深度ステンシル
	init.mDepthStencilDesc = DirectXCommon::gDepthEnable;
	// 頂点レイアウト
	init.mInputLayouts.push_back(mInputLayoutMap["POSITION"]);
	init.mInputLayouts.push_back(mInputLayoutMap["NORMAL"]);
	init.mInputLayouts.push_back(mInputLayoutMap["TEXCOORD"]);
	if ((flags & PSOFlags::kSkin) == PSOFlags::kSkin) {
		init.mInputLayouts.push_back(mInputLayoutMap["WEIGHT"]);
		init.mInputLayouts.push_back(mInputLayoutMap["BONE_INDEX"]);
	}
	// プリミティブ型
	init.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// パイプラインステートの作成と追加
	std::unique_ptr<PipelineState> ps = std::make_unique<PipelineState>();
	ps->Create(init);
	mPipelineStates[flags] = std::move(ps);
	return mPipelineStates[flags].get();
}

// ライトの更新
void ModelBase::UpdateLight() {
	// ライトの数(最大数は超えないように)
	LightCommon lightCommon = {};
	lightCommon.mDirectionalLightNum = MathUtil::Min(static_cast<uint32_t>(mDirectionalLights.size()), kMaxDirectionalLightNum);
	lightCommon.mPointLightNum = MathUtil::Min(static_cast<uint32_t>(mPointLights.size()), kMaxPointLightNum);
	lightCommon.mSpotLightNum = MathUtil::Min(static_cast<uint32_t>(mSpotLights.size()), kMaxSpotLightNum);
	mLightCommonBuff->Update(lightCommon);
	// 平行光源
	if (lightCommon.mDirectionalLightNum > 0) {
		std::vector<DirectionalLight> tmp(kMaxDirectionalLightNum);
		for (uint32_t i = 0; i < lightCommon.mDirectionalLightNum; ++i) {
			tmp[i].mColor = mDirectionalLights[i]->mColor;
			tmp[i].mIntensity = mDirectionalLights[i]->mIntensity;
			tmp[i].mDirection = Normalize(mDirectionalLights[i]->mDirection);// 正規化
		}
		mDirectionalLightBuff->Update(tmp.data());
	}
	// 点光源
	if (lightCommon.mPointLightNum > 0) {
		std::vector<PointLight> tmp(kMaxPointLightNum);
		for (uint32_t i = 0; i < lightCommon.mPointLightNum; ++i) {
			tmp[i].mColor = mPointLights[i]->mColor;
			tmp[i].mIntensity = mPointLights[i]->mIntensity;
			tmp[i].mPosition = mPointLights[i]->mPosition;
			tmp[i].mRadius = mPointLights[i]->mRadius;
			tmp[i].mDecay = mPointLights[i]->mDecay;
		}
		mPointLightBuff->Update(tmp.data());
	}
	// スポットライト
	if (lightCommon.mSpotLightNum > 0) {
		std::vector<SpotLight> tmp(kMaxSpotLightNum);
		for (uint32_t i = 0; i < lightCommon.mSpotLightNum; ++i) {
			tmp[i].mColor = mSpotLights[i]->mColor;
			tmp[i].mIntensity = mSpotLights[i]->mIntensity;
			tmp[i].mDirection = Normalize(mSpotLights[i]->mDirection);// 正規化
			tmp[i].mPosition = mSpotLights[i]->mPosition;
			tmp[i].mRadius = mSpotLights[i]->mRadius;
			tmp[i].mDecay = mSpotLights[i]->mDecay;
			tmp[i].mInner = std::cosf(mSpotLights[i]->mInner);// cosへ
			tmp[i].mOuter = std::cosf(mSpotLights[i]->mOuter);// cosへ
		}
		mSpotLightBuff->Update(tmp.data());
	}
}
