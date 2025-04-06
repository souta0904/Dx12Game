#pragma once
#include "CourseObj.h"

class Player;

// 敵
class Enemy : public CourseObj {
public:
	Enemy(GameScene* gameScene);
	void Initialize() override;
	void Update(InputBase* input, float deltaTime) override;
	void Draw() override;
	void OnCollision(CourseObj* obj) override;
private:
	std::unique_ptr<ModelInstance> mModel;
	std::unique_ptr<Material> mMaterial;
	Vector4 mNormalCol;
	Vector4 mDamageCol;
	float mDamageColTime;
};
