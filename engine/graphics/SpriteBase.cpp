#include "SpriteBase.h"
#include "directX/DirectXBase.h"
#include "directX/DirectXCommon.h"
#include "graphics/Camera.h"
#include "ResourceMgr.h"
#include <cassert>

void SpriteBase::Initialize() {
	// ルートシグネチャ作成
	mRootSignature = std::make_unique<RootSignature>(static_cast<uint32_t>(RootParamNum::kMax), 1);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kConstant)).InitCBV(0, D3D12_SHADER_VISIBILITY_ALL);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kTexture)).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mRootSignature->GetParameter(static_cast<uint32_t>(RootParamNum::kTexture)).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mRootSignature->GetSampler(0) = DirectXCommon::gSamplerLinearWrap;
	mRootSignature->Create();

	// パイプラインステート作成
	ResourceMgr& resourceMgr = ResourceMgr::GetInstance();
	PSOInit init;
	init.mRootSignature = mRootSignature.get();
	init.mVertexShader = resourceMgr.GetShader("resources/shaders/SpriteVS.hlsl", "vs_6_0");
	init.mPixelShader = resourceMgr.GetShader("resources/shaders/SpritePS.hlsl", "ps_6_0");
	init.mBlendDesc = DirectXCommon::gBlendNone;
	init.mRasterizerDesc = DirectXCommon::gRasterizerCullModeNone;
	init.mDepthStencilDesc = DirectXCommon::gDepthDisable;
	init.mInputLayouts.resize(2);
	init.mInputLayouts[0].SemanticName = "POSITION";
	init.mInputLayouts[0].SemanticIndex = 0;
	init.mInputLayouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	init.mInputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	init.mInputLayouts[1].SemanticName = "TEXCOORD";
	init.mInputLayouts[1].SemanticIndex = 0;
	init.mInputLayouts[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	init.mInputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	mPipelineStates[0] = std::make_unique<PipelineState>();
	mPipelineStates[0]->Create(init);
	// αブレンド
	init.mBlendDesc = DirectXCommon::gBlendNormal;
	mPipelineStates[1] = std::make_unique<PipelineState>();
	mPipelineStates[1]->Create(init);
	// 加算
	init.mBlendDesc = DirectXCommon::gBlendAddition;
	mPipelineStates[2] = std::make_unique<PipelineState>();
	mPipelineStates[2]->Create(init);
	// 減算
	init.mBlendDesc = DirectXCommon::gBlendSubtract;
	mPipelineStates[3] = std::make_unique<PipelineState>();
	mPipelineStates[3]->Create(init);
	// 乗算
	init.mBlendDesc = DirectXCommon::gBlendMultiply;
	mPipelineStates[4] = std::make_unique<PipelineState>();
	mPipelineStates[4]->Create(init);
	// スクリーン
	init.mBlendDesc = DirectXCommon::gBlendScreen;
	mPipelineStates[5] = std::make_unique<PipelineState>();
	mPipelineStates[5]->Create(init);

	// デフォルトのカメラ
	mDefaultCamera = std::make_unique<Camera>();
	mDefaultCamera->mProjectionMode = ProjectionMode::kOrthographic;
	mDefaultCamera->mNearZ = 0.0f;
	mDefaultCamera->mFarZ = 1.0f;
}

void SpriteBase::Terminate() {

}

// 描画準備
void SpriteBase::Prepare() {
	// カメラを更新
	mDefaultCamera->UpdateMatrix();

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = DirectXBase::GetInstance().GetCmdList();
	mRootSignature->Set(cmdList);
	mPipelineStates[static_cast<uint32_t>(BlendMode::kNormal)]->Set(cmdList);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

// パイプラインステートをセット
void SpriteBase::SetPipelineState(BlendMode blendMode) {
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = DirectXBase::GetInstance().GetCmdList();
	uint32_t idx = static_cast<uint32_t>(blendMode);
	assert(idx >= 0 && idx < static_cast<uint32_t>(BlendMode::kMax));
	mPipelineStates[idx]->Set(cmdList);
}
