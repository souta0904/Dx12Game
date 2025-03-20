#include "IFramework.h"
#include "directX/DirectXBase.h"
#include "graphics/LineRenderer.h"
#include "graphics/ModelBase.h"
#include "graphics/ResourceMgr.h"
#include "graphics/SpriteBase.h"
#include "Helper.h"
#include "Random.h"
#include "Window.h"
#include <format>

// 実行
void IFramework::Run() {
	Initialize();
	// メインループ
	while (!mWindow->ProcessMessage()) {
		Update();
		Draw();
	}
	Terminate();
}

// シーンの変更
void IFramework::ChangeScene(IScene* scene) {
	if (mCurrScene) {
		mCurrScene->Terminate();
	}
	if (scene) {
		mCurrScene.reset(scene);
		mCurrScene->Initialize();
	}
}

// 初期化
void IFramework::Initialize() {
	mWindow = &Window::GetInstance();
	// ウィンドウはmain.cppで作成
	if (!mWindow->GetHWnd()) {
		Helper::Log("Please create a window.");
	}

	mDirectXBase = &DirectXBase::GetInstance();
	mDirectXBase->Initialize();
	mImGuiWrapper = std::make_unique<ImGuiWrapper>();
	mImGuiWrapper->Initialize();
	mResourceMgr = &ResourceMgr::GetInstance();
	mResourceMgr->Initialize();
	mInput = std::make_unique<InputBase>();
	mInput->Initialize();
	Random::Initialize();

	mSpriteBase = &SpriteBase::GetInstance();
	mSpriteBase->Initialize();
	mModelBase = &ModelBase::GetInstance();
	mModelBase->Initialize();
	mLineRenderer = &LineRenderer::GetInstance();
	mLineRenderer->Initialize();

	Helper::Log(std::format("Initialize {} v.{}\n", ENGINE_NAME, ENGINE_VERSION));
}

// 終了処理
void IFramework::Terminate() {
	if (mCurrScene) {
		mCurrScene->Terminate();
	}

	if (mLineRenderer) {
		mLineRenderer->Terminate();
	}
	if (mModelBase) {
		mModelBase->Terminate();
	}
	if (mSpriteBase) {
		mSpriteBase->Terminate();
	}

	if (mInput) {
		mInput->Terminate();
	}
	if (mResourceMgr) {
		mResourceMgr->Terminate();
	}
	if (mImGuiWrapper) {
		mImGuiWrapper->Terminate();
	}
	if (mDirectXBase) {
		mDirectXBase->Terminate();
	}
}
