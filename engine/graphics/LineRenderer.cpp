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
	mRS->GetParameter(0).InitCBV(0, D3D12_SHADER_VISIBILITY_VERTEX);
	mRS->Create();

	// パイプラインステート作成
	ResourceMgr& resourceMgr = ResourceMgr::GetInstance();
	PSOInit init;
	init.mRootSignature = mRS.get();
	init.mVertexShader = resourceMgr.GetShader("resources/shaders/PrimitiveVS.hlsl", "vs_6_0");
	init.mPixelShader = resourceMgr.GetShader("resources/shaders/PrimitivePS.hlsl", "ps_6_0");
	init.mBlendDesc = DirectXCommon::gBlendNormal;
	init.mRasterizerDesc = DirectXCommon::gRasterizerCullModeNone;
	init.mDepthStencilDesc = DirectXCommon::gDepthEnable;
	init.mInputLayouts.resize(2);
	init.mInputLayouts[0].SemanticName = "POSITION";
	init.mInputLayouts[0].SemanticIndex = 0;
	init.mInputLayouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	init.mInputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	init.mInputLayouts[1].SemanticName = "COLOR";
	init.mInputLayouts[1].SemanticIndex = 0;
	init.mInputLayouts[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	init.mInputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	init.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	mPS = std::make_unique<PipelineState>();
	mPS->Create(init);

	// 定数バッファを作成
	mCB = std::make_unique<ConstantBuffer>();
	mCB->Create(sizeof(Matrix4));

	// 頂点を初期化
	mVB = std::make_unique<VertexBuffer>();
	mVB->Create(sizeof(Vertex) * kVertNum, sizeof(Vertex));
	mVertices.resize(kVertNum);
}

void LineRenderer::Terminate() {

}

void LineRenderer::Prepare() {
	// 頂点をリセット
	mVertIdx = 0;
	// カメラはモデルと共通
	Matrix4 vpMat = ModelBase::GetInstance().GetDefaultCamera()->GetViewProjectionMat();
	mCB->Update(vpMat);
}

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

void LineRenderer::DrawLine3(const Vector3& start, const Vector3& end, const Vector4& color) {
	mVertices[mVertIdx++] = { Vector4(start.x,start.y,start.z,1.0f),color };
	mVertices[mVertIdx++] = { Vector4(end.x,end.y,end.z,1.0f),color };
}

// グリッドを描画
void LineRenderer::DrawGrid(uint32_t dist) {
	// 何m先までか(1グリッド1m)
	float fGridDist = static_cast<float>(dist);
	// 横のライン
	for (uint32_t i = 0; i < dist * 2 + 1; ++i) {
		float z = i - fGridDist;
		if (i == dist) {// zが0.0のとき
			mVertices[mVertIdx++] = { Vector4(-fGridDist,0.0f,z,1.0f),Vector4(1.0f,0.2f,0.2f,1.0f) };// 赤
			mVertices[mVertIdx++] = { Vector4(+fGridDist,0.0f,z,1.0f),Vector4(1.0f,0.2f,0.2f,1.0f) };
		} else {
			mVertices[mVertIdx++] = { Vector4(-fGridDist,0.0f,z,1.0f),Vector4(1.0f,1.0f,1.0f,0.1f) };
			mVertices[mVertIdx++] = { Vector4(+fGridDist,0.0f,z,1.0f),Vector4(1.0f,1.0f,1.0f,0.1f) };
		}
	}
	// 縦のライン
	for (uint32_t i = 0; i < dist * 2 + 1; ++i) {
		float x = i - fGridDist;
		if (i == dist) {// xが0.0のとき
			mVertices[mVertIdx++] = { Vector4(x,0.0f,-fGridDist,1.0f),Vector4(0.2f,0.2f,1.0f,1.0f) };// 青
			mVertices[mVertIdx++] = { Vector4(x,0.0f,+fGridDist,1.0f),Vector4(0.2f,0.2f,1.0f,1.0f) };
		} else {
			mVertices[mVertIdx++] = { Vector4(x,0.0f,-fGridDist,1.0f),Vector4(1.0f,1.0f,1.0f,0.1f) };
			mVertices[mVertIdx++] = { Vector4(x,0.0f,+fGridDist,1.0f),Vector4(1.0f,1.0f,1.0f,0.1f) };
		}
	}
}
