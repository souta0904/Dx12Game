#pragma once
#include "IFramework.h"
#include "directX/PipelineState.h"
#include "graphics/Model.h"
#include "graphics/ModelBase.h"
#include "graphics/RenderTexture.h"
#include "Transform.h"
#include <chrono>
#include <memory>

class Game : public IFramework {
protected:
	void Initialize() override;
	void Terminate() override;
	void Update() override;
	void Draw()  override;

private:
	// デルタタイム
	std::chrono::steady_clock::time_point mPrevTime;
	float mDeltaTime;

	// シーンの描画先
	std::unique_ptr<RenderTexture> mSceneRT;
	// CRTポストエフェクト
	std::unique_ptr<RootSignature> mCRT_RS;
	std::unique_ptr<PipelineState> mCRT_PS;
	std::unique_ptr<ConstantBuffer> mCRT_CB;
	float mCRTTime;

	// ライト
	std::unique_ptr<DirectionalLight> mDirectionalLight;
};
