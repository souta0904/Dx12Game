#pragma once
#include "graphics/Model.h"
#include "Transform.h"
#include <cstdint>

class GameScene;
class InputBase;
class Course;
class Player;

// プレイヤー
class PlayerBullet {
public:
	PlayerBullet(Player* player);
	void Initialize();
	void Update(InputBase* input, float deltaTime);
	void Draw();

	bool GetIsDead() const { return mIsDead; }

private:
	Player* mPlayer;
	Transform mTransform;
	std::unique_ptr<ModelInstance> mModel;

	// コース
	Course* mCurrCourse;
	float mCurrT;

	float mSpeed = 1.0f;
	float mCourseRot;
	float mLife = 5.0f;
	bool mIsDead = false;
};
