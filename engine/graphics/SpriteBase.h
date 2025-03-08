#pragma once
#include "BlendMode.h"
#include "Camera.h"
#include "directX/PipelineState.h"
#include <memory>

class SpriteBase {
public:
	friend class Sprite;

	static SpriteBase& GetInstance() {
		static SpriteBase instance;
		return instance;
	}
	SpriteBase(const SpriteBase&) = delete;
	SpriteBase& operator=(const SpriteBase&) = delete;
	SpriteBase(SpriteBase&&) = delete;
	SpriteBase& operator=(SpriteBase&&) = delete;

	enum class RootParamIdx {
		kSprite,
		kTexture,
		kMax
	};

	void Initialize();
	void Terminate();
	void Prepare();
	void SetPipelineState(BlendMode blendMode);
	Camera* GetDefaultCamera() const { return mDefaultCamera.get(); }

private:
	SpriteBase() = default;
	~SpriteBase() = default;

private:
	std::unique_ptr<RootSignature> mRootSignature;
	std::unique_ptr<PipelineState> mPipelineStates[static_cast<uint32_t>(BlendMode::kMax)];
	std::unique_ptr<Camera> mDefaultCamera;
};
