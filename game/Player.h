#pragma once
#include "graphics/Model.h"
#include "PlayerBullet.h"
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

	const Transform& GetTransform() const { return mTransform; }
	Course* GetCurrCourse() const { return mCurrCourse; }
	float GetCurrT() const { return mCurrT; }
	float GetT2Cam() const { return mT2Cam; }
	float GetCourseRot() const { return mCourseRot; }

private:
	GameScene* mGameScene;
	Transform mTransform;
	std::unique_ptr<ModelInstance> mModel;
	std::vector<std::unique_ptr<PlayerBullet>> mBullets;
	float mCooldown;

	// コース
	Course* mCurrCourse;
	float mCurrT;
	// プレイヤーからカメラへのt
	float mT2Cam = -0.05f;

	float mSpeed;
	float mCourseRot;
	float mRotVel;
	float mRotSpeed;
};
