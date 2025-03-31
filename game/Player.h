#pragma once
#include "graphics/Model.h"
#include "Transform.h"
#include <cstdint>

class GameScene;
class InputBase;
class Course;

// プレイヤー
class Player {
public:
	Player(GameScene* gameScene);
	void Initialize();
	void Update(InputBase* input, float deltaTime);
	void Draw();

	float GetT() const { return mCurrT; }

private:
	GameScene* mGameScene;
	Transform mTransform;
	std::unique_ptr<ModelInstance> mModel;

	// コース
	Course* mCurrCourse;
	float mCurrT;

	float mSpeed;
	float mCourseRot;
	float mRotVel;
	float mRotSpeed;
};
