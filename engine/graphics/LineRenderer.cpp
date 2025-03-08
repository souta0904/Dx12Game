#include "LineRenderer.h"
#include "directX/DirectXBase.h"
#include "directX/DirectXCommon.h"
#include "Matrix4.h"
#include "ModelBase.h"
#include "ModelCamera.h"
#include "ResourceMgr.h"
#include "Window.h"

void LineRenderer::Initialize() {
	// ルートシグネチャ作成
	mRS = std::make_unique<RootSignature>(1, 0);
	mRS->Param(0).InitCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	mRS->Create();
	// パイプラインステート作成
	mPS = std::make_unique<PipelineState>();
	mPS->SetRootSignature(mRS.get());
	ResourceMgr& resourceMgr = ResourceMgr::GetInstance();
	mPS->SetVertexShader(resourceMgr.GetShader("resources/shaders/PrimitiveVS.hlsl", "vs_6_0"));
	mPS->SetPixelShader(resourceMgr.GetShader("resources/shaders/PrimitivePS.hlsl", "ps_6_0"));
	mPS->SetBlendState(DirectXCommon::gBlendNormal);
	mPS->SetRasterizerState(DirectXCommon::gRasterizerCullModeNone);
	mPS->SetDepthStencilState(DirectXCommon::gDepthEnable);
	D3D12_INPUT_ELEMENT_DESC inputLayouts[2] = {};
	inputLayouts[0].SemanticName = "POSITION";
	inputLayouts[0].SemanticIndex = 0;
	inputLayouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayouts[1].SemanticName = "COLOR";
	inputLayouts[1].SemanticIndex = 0;
	inputLayouts[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	mPS->SetInputLayout(_countof(inputLayouts), inputLayouts);
	mPS->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	mPS->Create();
	// 定数バッファ作成
	mCB = std::make_unique<ConstantBuffer>();
	mCB->Create(sizeof(Matrix4));
	// 頂点バッファ作成
	mVB = std::make_unique<VertexBuffer>();
	mVB->Create(sizeof(Vertex) * kVertNum, sizeof(Vertex));
	mVertices.resize(kVertNum);
}

void LineRenderer::Terminate() {

}

void LineRenderer::Prepare() {
	mVertIdx = 0;
	Matrix4 vp = ModelBase::GetInstance().GetDefaultCamera()->GetViewProjectionMat();
	mCB->Update(vp);
}

// まとめて描画
void LineRenderer::Render() {
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = DirectXBase::GetInstance().GetCmdList();
	mRS->Set(cmdList);
	mPS->Set(cmdList);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	mCB->Set(cmdList, 0);
	mVB->Update(mVertices.data());
	mVB->Set(cmdList);
	cmdList->DrawInstanced(mVertIdx, 1, 0, 0);
}

void LineRenderer::DrawLine3(const Vector3& p1, const Vector3& p2, const Vector4& color) {
	mVertices[mVertIdx++] = { Vector4(p1.x,p1.y,p1.z,1.0f),color };
	mVertices[mVertIdx++] = { Vector4(p2.x,p2.y,p2.z,1.0f),color };
}

// グリッドの描画
void LineRenderer::DrawGrid() {
	for (uint32_t i = 0; i < 1001; ++i) {// 横
		float z = i - 500.0f;
		if (i == 500) {// 真ん中は赤
			mVertices[mVertIdx++] = { Vector4(-500.0f,0.0f,z,1.0f),Vector4(1.0f,0.2f,0.2f,1.0f) };
			mVertices[mVertIdx++] = { Vector4(+500.0f,0.0f,z,1.0f),Vector4(1.0f,0.2f,0.2f,1.0f) };
		} else {
			mVertices[mVertIdx++] = { Vector4(-500.0f,0.0f,z,1.0f),Vector4(1.0f,1.0f,1.0f,0.1f) };
			mVertices[mVertIdx++] = { Vector4(+500.0f,0.0f,z,1.0f),Vector4(1.0f,1.0f,1.0f,0.1f) };
		}
	}
	for (uint32_t i = 0; i < 1001; ++i) {// 縦
		float x = i - 500.0f;
		if (i == 500) {// 真ん中は青
			mVertices[mVertIdx++] = { Vector4(x,0.0f,-500.0f,1.0f),Vector4(0.2f,0.2f,1.0f,1.0f) };
			mVertices[mVertIdx++] = { Vector4(x,0.0f,+500.0f,1.0f),Vector4(0.2f,0.2f,1.0f,1.0f) };
		} else {
			mVertices[mVertIdx++] = { Vector4(x,0.0f,-500.0f,1.0f),Vector4(1.0f,1.0f,1.0f,0.1f) };
			mVertices[mVertIdx++] = { Vector4(x,0.0f,+500.0f,1.0f),Vector4(1.0f,1.0f,1.0f,0.1f) };
		}
	}
}
