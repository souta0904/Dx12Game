#pragma once
#include "scene/IScene.h"
#include "Course.h"
#include "Enemy.h"
#include "Player.h"
#include <memory>

// ゲームプレイ
class GameScene : public IScene {
public:
	GameScene(IFramework* framework) : IScene(framework) {}
	virtual ~GameScene() {}
	void Initialize() override;
	void Terminate() override;
	void Update(InputBase* input, float deltaTime)  override;
	// 描画
	void DrawBackground() override;
	void DrawModel() override;
	void DrawPrimitive() override;
	void DrawForeground() override;

	Course* GetCourse1() const { return mCourse1.get(); }
	Course* GetCourse2() const { return mCourse2.get(); }
	Player* GetPlayer() const { return mPlayer.get(); }

private:
	std::unique_ptr<Course> mCourse1;
	std::unique_ptr<Course> mCourse2;
	std::unique_ptr<Player> mPlayer;
	std::vector<std::unique_ptr<Enemy>> mEnemies;
};
