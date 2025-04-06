#pragma once
#include "IFramework.h"
#include "directX/PipelineState.h"
#include "graphics/Model.h"
#include "graphics/ModelBase.h"
#include "graphics/RenderTexture.h"
#include "Transform.h"
#include <chrono>
#include <memory>

// ゲーム
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
	// ライト
	std::unique_ptr<DirectionalLight> mDirectionalLight;

	// シーンのレンダリング先
	std::unique_ptr<RenderTexture> mSceneRT;
	std::unique_ptr<RootSignature> mCopyRS;
	std::unique_ptr<PipelineState> mCopyPS;
	// ポストエフェクト
	bool mUsePE = true;
	std::unique_ptr<RootSignature> mCRT_RS;
	std::unique_ptr<PipelineState> mCRT_PS;
	// ポストエフェクト用定数
	struct CRT_Constant {
		float mTime;					// 時間
		float mZoom = 0.93f;			// 拡大縮小
		float mDistortion = 0.1f;		// 歪み
		float mNoise = 0.015f;			// ノイズ
		float mNoiseScaleX = 100.0f;	// ノイズスケールX
		float mNoiseScaleY = 800.0f;	// ノイズスケールY
		float mNoiseSpeed = 15.0f;		// ノイズの速さ
		float mPad1;
		Vector2 mRGBShift = Vector2(0.0013f, 0.0013f);// RGBずらし
		float mPad[2];
	};
	CRT_Constant mCRT_Constant;
	std::unique_ptr<ConstantBuffer> mCRT_CB;
};
