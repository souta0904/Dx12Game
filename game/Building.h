#pragma once
#include "graphics/Model.h"
#include "Transform.h"

class Building {
public:
	void Initialize(const Vector3& translate);
	void Draw();
private:
	std::unique_ptr<ModelInstance> mModel;
	Transform mTransform;
};
