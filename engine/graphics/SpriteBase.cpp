#include "SpriteBase.h"
#include "directX/DirectXBase.h"
#include "directX/DirectXCommon.h"
#include "graphics/Camera.h"
#include "ResourceMgr.h"
#include <cassert>

void SpriteBase::Initialize() {
	mRootSignature = std::make_unique<RootSignature>(static_cast<uint32_t>(RootParamIdx::kMax), 1);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kSprite)).InitCBV(0, D3D12_SHADER_VISIBILITY_ALL);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kTexture)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->Param(static_cast<uint32_t>(RootParamIdx::kTexture)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mRootSignature->Sampler(0) = DirectXCommon::gSamplerLinearWrap;
	mRootSignature->Create();

	mPipelineStates[0] = std::make_unique<PipelineState>();
	mPipelineStates[0]->SetRootSignature(mRootSignature.get());
	ResourceMgr& resourceMgr = ResourceMgr::GetInstance();
	mPipelineStates[0]->SetVertexShader(resourceMgr.GetShader("resources/shaders/SpriteVS.hlsl", "vs_6_0"));
	mPipelineStates[0]->SetPixelShader(resourceMgr.GetShader("resources/shaders/SpritePS.hlsl", "ps_6_0"));
	mPipelineStates[0]->SetBlendState(DirectXCommon::gBlendNone);
	mPipelineStates[0]->SetRasterizerState(DirectXCommon::gRasterizerCullModeNone);
	mPipelineStates[0]->SetDepthStencilState(DirectXCommon::gDepthDisable);
	D3D12_INPUT_ELEMENT_DESC inputLayouts[2] = {};
	inputLayouts[0].SemanticName = "POSITION";
	inputLayouts[0].SemanticIndex = 0;
	inputLayouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayouts[1].SemanticName = "TEXCOORD";
	inputLayouts[1].SemanticIndex = 0;
	inputLayouts[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	mPipelineStates[0]->SetInputLayout(_countof(inputLayouts), inputLayouts);
	mPipelineStates[0]->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	mPipelineStates[1] = std::make_unique<PipelineState>();
	*mPipelineStates[1] = *mPipelineStates[0];
	mPipelineStates[1]->SetBlendState(DirectXCommon::gBlendNormal);
	mPipelineStates[2] = std::make_unique<PipelineState>();
	*mPipelineStates[2] = *mPipelineStates[0];
	mPipelineStates[2]->SetBlendState(DirectXCommon::gBlendAddition);
	mPipelineStates[3] = std::make_unique<PipelineState>();
	*mPipelineStates[3] = *mPipelineStates[0];
	mPipelineStates[3]->SetBlendState(DirectXCommon::gBlendSubtract);
	mPipelineStates[4] = std::make_unique<PipelineState>();
	*mPipelineStates[4] = *mPipelineStates[0];
	mPipelineStates[4]->SetBlendState(DirectXCommon::gBlendMultiply);
	mPipelineStates[5] = std::make_unique<PipelineState>();
	*mPipelineStates[5] = *mPipelineStates[0];
	mPipelineStates[5]->SetBlendState(DirectXCommon::gBlendScreen);
	for (auto& pipelineState : mPipelineStates) {
		pipelineState->Create();
	}

	mDefaultCamera = std::make_unique<Camera>();
	mDefaultCamera->mProjectionMode = ProjectionMode::kOrthographic;
	mDefaultCamera->mNearZ = 0.0f;
	mDefaultCamera->mFarZ = 1.0f;
}

void SpriteBase::Terminate() {

}

void SpriteBase::Prepare() {
	mDefaultCamera->UpdateMatrix();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = DirectXBase::GetInstance().GetCmdList();
	mRootSignature->Set(cmdList);
	mPipelineStates[static_cast<uint32_t>(BlendMode::kNormal)]->Set(cmdList);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void SpriteBase::SetPipelineState(BlendMode blendMode) {
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = DirectXBase::GetInstance().GetCmdList();
	uint32_t idx = static_cast<uint32_t>(blendMode);
	assert(idx >= 0 && idx < static_cast<uint32_t>(BlendMode::kMax));
	mPipelineStates[idx]->Set(cmdList);
}
