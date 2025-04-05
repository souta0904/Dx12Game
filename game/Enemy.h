#pragma once
#include "CircleCollider.h"
#include "graphics/Model.h"
#include "Transform.h"
#include <cstdint>

class GameScene;
class InputBase;
class Course;
class Player;

// 敵
class Enemy : public CircleCollider {
public:
	Enemy(GameScene* gameScene);
	void Initialize(Course* course, float t, float rot);
	void Update(InputBase* input, float deltaTime);
	void Draw();

	void OnCollision() override { mIsDead = true; }

	bool GetIsDead() const { return mIsDead; }

private:
	GameScene* mGameScene;
	Transform mTransform;
	std::unique_ptr<ModelInstance> mModel;
	std::unique_ptr<Material> mMaterial;

	// コース
	Course* mCurrCourse;
	float mCurrT;

	float mSpeed = -0.1f;
	float mCourseRot;
	bool mIsDead = false;
};
