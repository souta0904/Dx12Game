#pragma once
#include "BlendMode.h"
#include "Camera.h"
#include "directX/PipelineState.h"
#include <memory>

// スプライト基盤
class SpriteBase {
public:
	friend class Sprite;
	// シングルトンを実装
	static SpriteBase& GetInstance() {
		static SpriteBase instance;
		return instance;
	}
	SpriteBase(const SpriteBase&) = delete;
	SpriteBase& operator=(const SpriteBase&) = delete;
	SpriteBase(SpriteBase&&) = delete;
	SpriteBase& operator=(SpriteBase&&) = delete;
private:
	SpriteBase() = default;
	~SpriteBase() = default;

public:
	// ルートパラメータ番号
	enum class RootParamNum {
		kConstant,
		kTexture,
		kMax
	};

	void Initialize();
	void Terminate();
	// スプライト描画準備
	void Prepare();
	// ブレンドモードでパイプラインステートをセット
	void SetPipelineState(BlendMode blendMode);

	Camera* GetDefaultCamera() const { return mDefaultCamera.get(); }

private:
	std::unique_ptr<RootSignature> mRootSignature;
	std::unique_ptr<PipelineState> mPipelineStates[static_cast<uint32_t>(BlendMode::kMax)];
	// デフォルトのカメラ
	std::unique_ptr<Camera> mDefaultCamera;
};
