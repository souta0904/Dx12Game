#pragma once
#include "scene/IScene.h"
#include "Course.h"
#include "CourseObj.h"
#include <memory>

class Player;

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

	void AddObject(std::unique_ptr<CourseObj> obj);
	Course* GetCurrCourse() const { return mCurrCourse; }
	Player* GetPlayer() const { return mPlayer; }

private:
	std::unique_ptr<Course> mCourse1;
	std::unique_ptr<Course> mCourse2;
	Course* mCurrCourse;
	bool mIsUpdating = false;
	std::vector<std::unique_ptr<CourseObj>> mObjects;
	std::vector<std::unique_ptr<CourseObj>> mWaitObjs;
	Player* mPlayer;
};
