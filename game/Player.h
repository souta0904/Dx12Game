#pragma once
#include "Transform.h"
#include "graphics/Model.h"
#include <cstdint>

class InputBase;
class Course;

// プレイヤー
class Player {
public:
	void Initialize();
	void Update(InputBase* input, float deltaTime);
	void Draw();

	void SetCource(Course* cource) { mCurrCource = cource; }

private:
	Transform mTransform;
	// コース関連
	Course* mCurrCource;// 現在のコース
	uint32_t mCurrSection;// 現在の区間
	float mT;// パラメータ
	float mSpeed;// パラメータtの速さ
	float mAngle;// 角度
	float mAngleVel;// 角速度
	float mAngleSpeed;
	// モデル
	std::unique_ptr<ModelInstance> mModel;
};
