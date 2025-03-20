#pragma once
#include "Transform.h"
#include "graphics/Model.h"
#include <cstdint>

class InputBase;
class Course;

class Player {
public:
	void Initialize();
	void Update(InputBase* input, float deltaTime);
	void Draw();
	void SetRing(Course* cource) { mCource = cource; }
private:
	Transform mTransform;
	Course* mCource;
	uint32_t mSection;
	float mT;
	float mSpeed;
	float mRadian;
	float mAngle;
	std::unique_ptr<ModelInstance> mModel;
};
