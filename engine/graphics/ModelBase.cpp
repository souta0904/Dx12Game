#include "ModelBase.h"
#include "directX/DirectXBase.h"
#include "directX/DirectXCommon.h"
#include "ResourceMgr.h"

void ModelBase::Initialize() {
	mRootSignature = std::make_unique<RootSignature>(static_cast<uint32_t>(RootParamIdx::kMax), 1);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kTransformationMat)).InitCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kMaterial)).InitCBV(0, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kTexture)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kTexture)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kCamera)).InitCBV(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kLightCommon)).InitCBV(2, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kDirectionalLight)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kDirectionalLight)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kPointLight)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kPointLight)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kSpotLight)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kSpotLight)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kSkin)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_VERTEX);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kSkin)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mRootSignature->Sampler(0) = DirectXCommon::gSamplerLinearClamp;
	mRootSignature->Create();

	D3D12_INPUT_ELEMENT_DESC inputElementDesc = {};
	inputElementDesc.SemanticName = "POSITION";
	inputElementDesc.SemanticIndex = 0;
	inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	mInputLayouts.emplace(inputElementDesc.SemanticName, inputElementDesc);
	inputElementDesc.SemanticName = "NORMAL";
	inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	mInputLayouts.emplace(inputElementDesc.SemanticName, inputElementDesc);
	inputElementDesc.SemanticName = "TEXCOORD";
	inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	mInputLayouts.emplace(inputElementDesc.SemanticName, inputElementDesc);
	inputElementDesc.SemanticName = "WEIGHT";
	inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	mInputLayouts.emplace(inputElementDesc.SemanticName, inputElementDesc);
	inputElementDesc.SemanticName = "BONE_INDEX";
	inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
	mInputLayouts.emplace(inputElementDesc.SemanticName, inputElementDesc);

	CreatePipelineState(PSOFlags::kNone);
	CreatePipelineState(PSOFlags::kHasTexture);
	CreatePipelineState(PSOFlags::kHasSkin);
	CreatePipelineState(PSOFlags::kHasTexture | PSOFlags::kHasSkin);

	mLightCommonBuff = std::make_unique<ConstantBuffer>();
	mLightCommonBuff->Create(sizeof(LightCommon));
	mDirectionalLightBuff = std::make_unique<StructuredBuffer>();
	mDirectionalLightBuff->Create(kMaxDirectionalLightNum, sizeof(DirectionalLight));
	mPointLightBuff = std::make_unique<StructuredBuffer>();
	mPointLightBuff->Create(kMaxPointLightNum, sizeof(PointLight));
	mSpotLightBuff = std::make_unique<StructuredBuffer>();
	mSpotLightBuff->Create(kMaxSpotLightNum, sizeof(SpotLight));

	mDefaultCamera = std::make_unique<ModelCamera>();
	mDefaultCamera->Create();
	mDefaultCamera->mTranslate = Vector3(0.0f, 1.0f, -3.0f);
}

void ModelBase::Terminate() {

}

void ModelBase::Prepare() {
	mDefaultCamera->UpdateMatrix();

	LightCommon common = {};
	common.mDirectionalLightNum = MathUtil::Min(static_cast<uint32_t>(mDirectionalLights.size()), kMaxDirectionalLightNum);
	common.mPointLightNum = MathUtil::Min(static_cast<uint32_t>(mPointLights.size()), kMaxPointLightNum);
	common.mSpotLightNum = MathUtil::Min(static_cast<uint32_t>(mSpotLights.size()), kMaxSpotLightNum);
	mLightCommonBuff->Update(common);
	if (common.mDirectionalLightNum > 0) {
		std::vector<DirectionalLight> tmp(kMaxDirectionalLightNum);
		for (uint32_t i = 0; i < common.mDirectionalLightNum; ++i) {
			tmp[i].mColor = mDirectionalLights[i]->mColor;
			tmp[i].mIntensity = mDirectionalLights[i]->mIntensity;
			tmp[i].mDirection = Normalize(mDirectionalLights[i]->mDirection);
		}
		mDirectionalLightBuff->Update(tmp.data());
	}
	if (common.mPointLightNum > 0) {
		std::vector<PointLight> tmp(kMaxPointLightNum);
		for (uint32_t i = 0; i < common.mPointLightNum; ++i) {
			tmp[i].mColor = mPointLights[i]->mColor;
			tmp[i].mIntensity = mPointLights[i]->mIntensity;
			tmp[i].mPosition = mPointLights[i]->mPosition;
			tmp[i].mRadius = mPointLights[i]->mRadius;
			tmp[i].mDecay = mPointLights[i]->mDecay;
		}
		mPointLightBuff->Update(tmp.data());
	}
	if (common.mSpotLightNum > 0) {
		std::vector<SpotLight> tmp(kMaxSpotLightNum);
		for (uint32_t i = 0; i < common.mSpotLightNum; ++i) {
			tmp[i].mColor = mSpotLights[i]->mColor;
			tmp[i].mIntensity = mSpotLights[i]->mIntensity;
			tmp[i].mDirection = Normalize(mSpotLights[i]->mDirection);
			tmp[i].mPosition = mSpotLights[i]->mPosition;
			tmp[i].mRadius = mSpotLights[i]->mRadius;
			tmp[i].mDecay = mSpotLights[i]->mDecay;
			tmp[i].mInner = std::cosf(mSpotLights[i]->mInner);
			tmp[i].mOuter = std::cosf(mSpotLights[i]->mOuter);
		}
		mSpotLightBuff->Update(tmp.data());
	}

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = DirectXBase::GetInstance().GetCmdList();
	mRootSignature->Set(cmdList);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mLightCommonBuff->Set(cmdList, static_cast<uint32_t>(RootParamIdx::kLightCommon));
	mDirectionalLightBuff->Set(cmdList, static_cast<uint32_t>(RootParamIdx::kDirectionalLight));
	mPointLightBuff->Set(cmdList, static_cast<uint32_t>(RootParamIdx::kPointLight));
	mSpotLightBuff->Set(cmdList, static_cast<uint32_t>(RootParamIdx::kSpotLight));
}

void ModelBase::SetPipelineState(PSOFlags flags) {
	PipelineState* pipelineState = GetPipelineState(flags);
	if (pipelineState) {
		pipelineState->Set(DirectXBase::GetInstance().GetCmdList());
	}
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

PipelineState* ModelBase::GetPipelineState(PSOFlags flags) {
	auto it = mPipelineStates.find(flags);
	if (it != mPipelineStates.end()) {
		return it->second.get();
	} else {
		return CreatePipelineState(flags);
	}
}

PipelineState* ModelBase::CreatePipelineState(PSOFlags flags) {
	std::unique_ptr<PipelineState> pipelineState = std::make_unique<PipelineState>();
	pipelineState->SetRootSignature(mRootSignature.get());
	ResourceMgr& resourceMgr = ResourceMgr::GetInstance();
	if ((flags & PSOFlags::kHasSkin) == PSOFlags::kHasSkin) {
		pipelineState->SetVertexShader(resourceMgr.GetShader("resources/shaders/SkinModelVS.hlsl", "vs_6_0"));
	} else {
		pipelineState->SetVertexShader(resourceMgr.GetShader("resources/shaders/ModelVS.hlsl", "vs_6_0"));
	}
	if ((flags & PSOFlags::kHasTexture) == PSOFlags::kHasTexture) {
		pipelineState->SetPixelShader(resourceMgr.GetShader("resources/shaders/ModelPS.hlsl", "ps_6_0"));
	} else {
		pipelineState->SetPixelShader(resourceMgr.GetShader("resources/shaders/NoTextureModelPS.hlsl", "ps_6_0"));
	}
	if ((flags & PSOFlags::kAlphaBlend) == PSOFlags::kAlphaBlend) {
		pipelineState->SetBlendState(DirectXCommon::gBlendNormal);
	} else {
		pipelineState->SetBlendState(DirectXCommon::gBlendNone);
	}
	D3D12_RASTERIZER_DESC rasterizerDesc = DirectXCommon::gRasterizerDefault;
	if ((flags & PSOFlags::kCurrNone) == PSOFlags::kCurrNone) {
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	}
	if ((flags & PSOFlags::kWireframe) == PSOFlags::kWireframe) {
		rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}
	pipelineState->SetRasterizerState(rasterizerDesc);
	pipelineState->SetDepthStencilState(DirectXCommon::gDepthEnable);
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayouts;
	inputLayouts.push_back(mInputLayouts["POSITION"]);
	inputLayouts.push_back(mInputLayouts["NORMAL"]);
	inputLayouts.push_back(mInputLayouts["TEXCOORD"]);
	if ((flags & PSOFlags::kHasSkin) == PSOFlags::kHasSkin) {
		inputLayouts.push_back(mInputLayouts["WEIGHT"]);
		inputLayouts.push_back(mInputLayouts["BONE_INDEX"]);
	}
	pipelineState->SetInputLayout(static_cast<uint32_t>(inputLayouts.size()), inputLayouts.data());
	pipelineState->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	pipelineState->Create();
	mPipelineStates[flags] = std::move(pipelineState);
	return mPipelineStates[flags].get();
}
