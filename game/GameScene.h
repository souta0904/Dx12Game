#pragma once
#include "scene/IScene.h"
#include "Player.h"
#include "Course.h"
#include <memory>

// ゲームプレイ
class GameScene : public IScene {
public:
	GameScene(IFramework* framework) : IScene(framework) {}
	virtual ~GameScene() {}
	void Initialize() override;
	void Terminate() override;
	void Update(InputBase* input, float deltaTime)  override;

	void DrawBackground() override;
	void DrawModel() override;
	void DrawPrimitive() override;
	void DrawForeground() override;

private:
	std::unique_ptr<Course> mCourse;
	std::unique_ptr<Player> mPlayer;
};
