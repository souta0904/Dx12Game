#include "PipelineState.h"
#include "DirectXBase.h"
#include <cassert>

RootParameter::~RootParameter() {
	if (mRootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
		delete[] mRootParam.DescriptorTable.pDescriptorRanges;
	}
}

void RootParameter::InitCBV(uint32_t registerNum, D3D12_SHADER_VISIBILITY shaderVisibility) {
	mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	mRootParam.Constants.ShaderRegister = registerNum;
	mRootParam.Constants.RegisterSpace = 0;
	mRootParam.Constants.Num32BitValues = 0;
	mRootParam.ShaderVisibility = shaderVisibility;
}

void RootParameter::InitDescriptorTable(uint32_t rangeNum, D3D12_SHADER_VISIBILITY shaderVisibility) {
	mRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	mRootParam.DescriptorTable.NumDescriptorRanges = rangeNum;
	mRootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[rangeNum];
	mRootParam.ShaderVisibility = shaderVisibility;
}

void RootParameter::InitDescriptorRange(uint32_t idx, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t numDescs, uint32_t registerNum) {
	D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(&mRootParam.DescriptorTable.pDescriptorRanges[idx]);
	range->RangeType = rangeType;
	range->NumDescriptors = numDescs;
	range->BaseShaderRegister = registerNum;
	range->RegisterSpace = 0;
	range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

RootSignature::RootSignature(uint32_t numParams, uint32_t numSamplers) {
	mNumParams = numParams;
	if (mNumParams > 0) {
		mParams.reset(new RootParameter[mNumParams]);
	}
	mNumSamplers = numSamplers;
	if (mNumSamplers > 0) {
		mSamplers.reset(new D3D12_STATIC_SAMPLER_DESC[mNumSamplers]);
	}
}

RootParameter& RootSignature::Param(uint32_t idx) {
	assert(idx >= 0 && idx < mNumParams);
	return mParams[idx];
}

D3D12_STATIC_SAMPLER_DESC& RootSignature::Sampler(uint32_t idx) {
	assert(idx >= 0 && idx < mNumSamplers);
	return mSamplers[idx];
}

void RootSignature::Create() {
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	desc.NumParameters = mNumParams;
	if (mNumParams > 0) {
		desc.pParameters = &(mParams[0].mRootParam);
	}
	desc.NumStaticSamplers = mNumSamplers;
	desc.pStaticSamplers = mSamplers.get();
	Microsoft::WRL::ComPtr<ID3DBlob> rsBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rsBlob, &errorBlob);
	assert(SUCCEEDED(hr));
	hr = DirectXBase::GetInstance().GetDevice()->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
	assert(SUCCEEDED(hr));
}

void RootSignature::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	cmdList->SetGraphicsRootSignature(mRootSignature.Get());
}

PipelineState::PipelineState() {
	mDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	mDesc.NumRenderTargets = 1;
	mDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	mDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	mDesc.SampleDesc.Count = 1;
}

PipelineState::PipelineState(const PipelineState& ps) {
	mDesc = ps.mDesc;
	mInputLayouts = ps.mInputLayouts;
	mDesc.InputLayout.pInputElementDescs = mInputLayouts.data();
}

PipelineState& PipelineState::operator=(const PipelineState& ps) {
	mDesc = ps.mDesc;
	mInputLayouts = ps.mInputLayouts;
	mDesc.InputLayout.pInputElementDescs = mInputLayouts.data();
	return *this;
}

void PipelineState::SetRootSignature(RootSignature* rs) {
	if (rs) {
		mDesc.pRootSignature = rs->GetRootSignature().Get();
	}
}

void PipelineState::SetVertexShader(Microsoft::WRL::ComPtr<IDxcBlob> vs) {
	assert(vs);
	mDesc.VS.pShaderBytecode = vs->GetBufferPointer();
	mDesc.VS.BytecodeLength = vs->GetBufferSize();
}

void PipelineState::SetPixelShader(Microsoft::WRL::ComPtr<IDxcBlob> ps) {
	assert(ps);
	mDesc.PS.pShaderBytecode = ps->GetBufferPointer();
	mDesc.PS.BytecodeLength = ps->GetBufferSize();
}

void PipelineState::SetBlendState(D3D12_BLEND_DESC bs) {
	mDesc.BlendState = bs;
}

void PipelineState::SetRasterizerState(D3D12_RASTERIZER_DESC rs) {
	mDesc.RasterizerState = rs;
}

void PipelineState::SetDepthStencilState(D3D12_DEPTH_STENCIL_DESC dss) {
	mDesc.DepthStencilState = dss;
}

void PipelineState::SetInputLayout(uint32_t num, const D3D12_INPUT_ELEMENT_DESC* descs) {
	mDesc.InputLayout.NumElements = num;
	if (num > 0) {
		mInputLayouts.resize(num);
		for (uint32_t i = 0; i < num; ++i) {
			mInputLayouts[i] = descs[i];
		}
		mDesc.InputLayout.pInputElementDescs = mInputLayouts.data();
	}
}

void PipelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE ptt) {
	mDesc.PrimitiveTopologyType = ptt;
}

void PipelineState::Create() {
	[[maybe_unused]] HRESULT hr = DirectXBase::GetInstance().GetDevice()->CreateGraphicsPipelineState(&mDesc, IID_PPV_ARGS(&mPipelineState));
	assert(SUCCEEDED(hr));
}

void PipelineState::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	cmdList->SetPipelineState(mPipelineState.Get());
}
