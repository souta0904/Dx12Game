#pragma once
#include "ImGuiWrapper.h"
#include "input/InputBase.h"
#include "scene/IScene.h"
#include <memory>
#define ENGINE_NAME "RD ENGINE"// エンジン名
#define ENGINE_VERSION "1.0.0"// バージョン

class Window;
class DirectXBase;
class ResourceMgr;
class SpriteBase;
class ModelBase;
class LineRenderer;

// 基盤
class IFramework {
public:
	void Run();
	// シーンの変更
	void ChangeScene(IScene* scene);

protected:
	// オーバーライド用
	virtual void Initialize();
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Terminate();

protected:
	bool mIsRunning = true;// 実行中か
	Window* mWindow;// ウィンドウ
	DirectXBase* mDirectXBase;// DirectX基盤
	std::unique_ptr<ImGuiWrapper> mImGuiWrapper;
	ResourceMgr* mResourceMgr;// リソース管理
	std::unique_ptr<InputBase> mInput;// 入力
	// レンダリング関連
	SpriteBase* mSpriteBase;// スプライト
	ModelBase* mModelBase;// モデル
	LineRenderer* mLineRenderer;// ライン描画

	// 現在のシーン
	std::unique_ptr<IScene> mCurrScene;
};
