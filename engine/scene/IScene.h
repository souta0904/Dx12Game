#pragma once

class IFramework;
class InputBase;

// シーンのインターフェース
class IScene {
public:
	IScene(IFramework* framework) : mFramework(framework) {}
	virtual ~IScene() {}
	// 初期化
	virtual void Initialize() = 0;
	// 更新
	virtual void Update(InputBase* input, float deltaTime) = 0;
	// 描画
	virtual void DrawBackground() = 0;// 背景スプライト
	virtual void DrawModel() = 0;// モデル
	virtual void DrawPrimitive() = 0;// プリミティブ(今はラインのみ)
	virtual void DrawForeground() = 0;// 前景スプライト
	// 終了処理
	virtual void Terminate() = 0;
private:
	IFramework* mFramework;
};
