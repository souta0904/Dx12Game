#pragma once
#include "CourseObj.h"
#include "PlayerBullet.h"

// プレイヤー
class Player : public CourseObj {
public:
	Player(GameScene* gameScene);
	void Initialize() override;
	void Update(InputBase* input, float deltaTime) override;
	void Draw() override;
	void OnCollision(CourseObj*) override {}
	float GetT2Cam() const { return mT2Cam; }
private:
	std::unique_ptr<ModelInstance> mModel;
	float mHP;
	float mRotVel;
	float mRotSpeed;
	// カメラへのt
	float mT2Cam = -0.15f;
	// 弾
	float mCooldown;
};
