#include "Game.h"
#include "directX/DirectXBase.h"
#include "directX/DirectXCommon.h"
#include "GameScene.h"
#include "graphics/LineRenderer.h"
#include "graphics/ModelBase.h"
#include "graphics/ResourceMgr.h"
#include "graphics/SpriteBase.h"
#include "Matrix4.h"
#include "Window.h"

void Game::Initialize() {
	IFramework::Initialize();

	// シーンのレンダリング先
	mSceneRT = std::make_unique<RenderTexture>();
	Window& window = Window::GetInstance();
	mSceneRT->Create(window.GetClientWidth(), window.GetClientHeight(), true);

	// CopyImage
	mCopyRS = std::make_unique<RootSignature>(1, 1);
	mCopyRS->GetParameter(0).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mCopyRS->GetParameter(0).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mCopyRS->GetSampler(0) = DirectXCommon::gSamplerLinearWrap;
	mCopyRS->Create();
	PSOInit init;
	init.mRootSignature = mCopyRS.get();
	ResourceMgr& resourceMgr = ResourceMgr::GetInstance();
	init.mVertexShader = resourceMgr.GetShader("resources/shaders/CopyImageVS.hlsl", "vs_6_0");
	init.mPixelShader = resourceMgr.GetShader("resources/shaders/CopyImagePS.hlsl", "ps_6_0");
	init.mBlendDesc = DirectXCommon::gBlendNone;
	init.mRasterizerDesc = DirectXCommon::gRasterizerCullModeNone;
	init.mDepthStencilDesc = DirectXCommon::gDepthDisable;
	init.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	mCopyPS = std::make_unique<PipelineState>();
	mCopyPS->Create(init);

	// ポストエフェクト
	mCRT_RS = std::make_unique<RootSignature>(2, 1);
	mCRT_RS->GetParameter(0).InitDescriptorTable(1, D3D12_SHADER_VISIBILITY_PIXEL);
	mCRT_RS->GetParameter(0).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mCRT_RS->GetParameter(1).InitCBV(0, D3D12_SHADER_VISIBILITY_ALL);
	mCRT_RS->GetSampler(0) = DirectXCommon::gSamplerLinearWrap;
	mCRT_RS->Create();
	init.mRootSignature = mCRT_RS.get();
	init.mPixelShader = resourceMgr.GetShader("resources/shaders/CathodeRayTubePS.hlsl", "ps_6_0");
	mCRT_PS = std::make_unique<PipelineState>();
	mCRT_PS->Create(init);
	mCRT_CB = std::make_unique<ConstantBuffer>();
	mCRT_CB->Create(sizeof(CRT_Constant));

	// デフォルトのライト
	mDirectionalLight = std::make_unique<DirectionalLight>();
	mDirectionalLight->mColor = Vector3(1.0f, 1.0f, 1.0f);
	mDirectionalLight->mIntensity = 1.0f;
	mDirectionalLight->mDirection = Normalize(Vector3(0.6f, -1.0f, 0.8f));
	mModelBase->AddDirectionalLight(mDirectionalLight.get());

	// 最初のシーン
	ChangeScene(new GameScene(this));

	mPrevTime = std::chrono::steady_clock::now();
}

void Game::Terminate() {
	IFramework::Terminate();
}

void Game::Update() {
	mImGuiWrapper->Begin();
	mInput->Update();

	// デルタタイム計算
	std::chrono::steady_clock::time_point currTime = std::chrono::steady_clock::now();
	std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(currTime - mPrevTime);
	mDeltaTime = static_cast<float>(duration.count()) / 1000000.0f;
	mDeltaTime = MathUtil::Min(mDeltaTime, 0.05f);
	mPrevTime = currTime;
	// シーン更新
	if (mCurrScene) {
		mCurrScene->Update(mInput.get(), mDeltaTime);
	}

	// ポストエフェクト用定数を更新
	mCRT_Constant.mTime += mDeltaTime;
	ImGui::Begin("Post effect");
	ImGui::Checkbox("Use", &mUsePE);
	if (ImGui::Button("Reset")) {
		mCRT_Constant = CRT_Constant();
	}
	ImGui::DragFloat("Zoom", &mCRT_Constant.mZoom, 0.001f);
	ImGui::DragFloat("Distortion", &mCRT_Constant.mDistortion, 0.001f);
	ImGui::DragFloat("Noise", &mCRT_Constant.mNoise, 0.001f);
	ImGui::DragFloat("Noise scale x", &mCRT_Constant.mNoiseScaleX, 1.0f);
	ImGui::DragFloat("Noise scale y", &mCRT_Constant.mNoiseScaleY, 1.0f);
	ImGui::DragFloat("Noise speed", &mCRT_Constant.mNoiseSpeed, 1.0f);
	ImGui::DragFloat2("RGB shift", &mCRT_Constant.mRGBShift.x, 0.00001f, 0.0f, 1.0f, "%8.6f");
	ImGui::End();

	mImGuiWrapper->End();
}

void Game::Draw() {
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = mDirectXBase->GetCmdList();
	mDirectXBase->GetSRVHeap()->Set(cmdList);
	mSceneRT->Begin(cmdList);

	// 背景スプライトの描画
	SpriteBase& spriteBase = SpriteBase::GetInstance();
	spriteBase.Prepare();
	if (mCurrScene) {
		mCurrScene->DrawBackground();
	}

	// 3Dモデルの描画
	ModelBase& modelBase = ModelBase::GetInstance();
	modelBase.Prepare();
	if (mCurrScene) {
		mCurrScene->DrawModel();
	}

	// プリミティブの描画
	LineRenderer& lineRenderer = LineRenderer::GetInstance();
	lineRenderer.Prepare();
	if (mCurrScene) {
		mCurrScene->DrawPrimitive();
	}
	// グリッドの描画
	mLineRenderer->DrawGrid();
	mLineRenderer->Render();

	// 前景スプライトの描画
	spriteBase.Prepare();
	if (mCurrScene) {
		mCurrScene->DrawForeground();
	}

	mSceneRT->End(cmdList);
	// 最終的なレンダーターゲットへ描画
	mDirectXBase->SetFinalRT();
	if (mUsePE) {
		mCRT_RS->Set(cmdList);
		mCRT_PS->Set(cmdList);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mSceneRT->Set(cmdList, 0);
		mCRT_CB->Update(mCRT_Constant);
		mCRT_CB->Set(cmdList, 1);
	} else {
		mCopyRS->Set(cmdList);
		mCopyPS->Set(cmdList);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mSceneRT->Set(cmdList, 0);
	}
	cmdList->DrawInstanced(3, 1, 0, 0);// ポストエフェクト
	mImGuiWrapper->Draw(cmdList);// ImGui
	mDirectXBase->ExecuteCmdList();
}
