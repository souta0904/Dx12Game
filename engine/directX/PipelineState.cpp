#include "PipelineState.h"
#include "DirectXBase.h"
#include "graphics/ResourceMgr.h"
#include <cassert>

// ルートパラメータ
RootParameter::~RootParameter() {
	if (mRootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
		delete[] mRootParameter.DescriptorTable.pDescriptorRanges;
	}
}

// 定数バッファビューとして初期化
void RootParameter::InitCBV(uint32_t registerNum, D3D12_SHADER_VISIBILITY shaderVisibility) {
	mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	mRootParameter.Constants.ShaderRegister = registerNum;
	mRootParameter.Constants.RegisterSpace = 0;
	mRootParameter.Constants.Num32BitValues = 0;
	mRootParameter.ShaderVisibility = shaderVisibility;
}

// デスクリプタヒープとして初期化
void RootParameter::InitDescriptorTable(uint32_t rangeNum, D3D12_SHADER_VISIBILITY shaderVisibility) {
	mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	mRootParameter.DescriptorTable.NumDescriptorRanges = rangeNum;
	mRootParameter.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[rangeNum];
	mRootParameter.ShaderVisibility = shaderVisibility;
}

// デスクリプタレンジを初期化
void RootParameter::InitDescriptorRange(uint32_t idx, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t descNum, uint32_t baseRegisterNum) {
	D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(&mRootParameter.DescriptorTable.pDescriptorRanges[idx]);
	range->RangeType = rangeType;
	range->NumDescriptors = descNum;
	range->BaseShaderRegister = baseRegisterNum;
	range->RegisterSpace = 0;
	range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

// ルートシグネチャ
RootSignature::RootSignature(uint32_t parameterNum, uint32_t samplerNum) {
	mParameterNum = parameterNum;
	if (mParameterNum > 0) {
		mParameters.reset(new RootParameter[mParameterNum]);
	}
	mSamplerNum = samplerNum;
	if (mSamplerNum > 0) {
		mSamplers.reset(new D3D12_STATIC_SAMPLER_DESC[mSamplerNum]);
	}
}

// ルートパラメータにアクセス
RootParameter& RootSignature::GetParameter(uint32_t idx) {
	assert(idx >= 0 && idx < mParameterNum);
	return mParameters[idx];
}

// サンプラーにアクセス
D3D12_STATIC_SAMPLER_DESC& RootSignature::GetSampler(uint32_t idx) {
	assert(idx >= 0 && idx < mSamplerNum);
	return mSamplers[idx];
}

void RootSignature::Create() {
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	desc.NumParameters = mParameterNum;
	if (mParameterNum > 0) {
		desc.pParameters = &(mParameters[0].mRootParameter);
	}
	desc.NumStaticSamplers = mSamplerNum;
	desc.pStaticSamplers = mSamplers.get();
	Microsoft::WRL::ComPtr<ID3DBlob> rsBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rsBlob, &errorBlob);
	assert(SUCCEEDED(hr));
	// ルートシグネチャの作成
	hr = DirectXBase::GetInstance().GetDevice()->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
	assert(SUCCEEDED(hr));
}

void RootSignature::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	cmdList->SetGraphicsRootSignature(mRootSignature.Get());
}

// パイプラインステート
PipelineState::PipelineState() {
	// デフォルトの設定
	mDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	mDesc.NumRenderTargets = 1;
	mDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	mDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	mDesc.SampleDesc.Count = 1;
}

// コピー、代入の定義
PipelineState::PipelineState(const PipelineState& pipelineState) {
	*this = pipelineState;
}
PipelineState& PipelineState::operator=(const PipelineState& pipelineState) {
	mDesc = pipelineState.mDesc;
	mInputLayouts = pipelineState.mInputLayouts;
	mDesc.InputLayout.pInputElementDescs = mInputLayouts.data();
	return *this;
}

void PipelineState::Create(const PSOInit& init) {
	mDesc.pRootSignature = init.mRootSignature->GetRootSignature().Get();
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = init.mVertexShader->GetBlob();
	mDesc.VS.pShaderBytecode = shaderBlob->GetBufferPointer();
	mDesc.VS.BytecodeLength = shaderBlob->GetBufferSize();
	shaderBlob = init.mPixelShader->GetBlob();
	mDesc.PS.pShaderBytecode = shaderBlob->GetBufferPointer();
	mDesc.PS.BytecodeLength = shaderBlob->GetBufferSize();
	mDesc.BlendState = init.mBlendDesc;
	mDesc.RasterizerState = init.mRasterizerDesc;
	mDesc.DepthStencilState = init.mDepthStencilDesc;
	mDesc.PrimitiveTopologyType = init.mPrimitiveTopologyType;
	// 頂点レイアウト
	uint32_t inputLayoutNum = static_cast<uint32_t>(init.mInputLayouts.size());
	mDesc.InputLayout.NumElements = inputLayoutNum;
	if (inputLayoutNum > 0) {
		mInputLayouts = init.mInputLayouts;
		mDesc.InputLayout.pInputElementDescs = mInputLayouts.data();
	}
	// パイプラインステートの作成
	[[maybe_unused]] HRESULT hr = DirectXBase::GetInstance().GetDevice()->CreateGraphicsPipelineState(&mDesc, IID_PPV_ARGS(&mPipelineState));
	assert(SUCCEEDED(hr));
}

void PipelineState::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	cmdList->SetPipelineState(mPipelineState.Get());
}
