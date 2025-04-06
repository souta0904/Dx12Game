#pragma once
#include "CourseObj.h"

class Player;

// プレイヤーの弾
class PlayerBullet : public CourseObj {
public:
	PlayerBullet(GameScene* gameScene);
	void Initialize() override;
	void Update(InputBase* input, float deltaTime) override;
	void Draw() override;
	void OnCollision(CourseObj* obj) override;
private:
	std::unique_ptr<ModelInstance> mModel;
	std::unique_ptr<Material> mMaterial;
	float mLife = 5.0f;
};
