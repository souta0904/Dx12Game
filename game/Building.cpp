#include "Building.h"
#include "graphics/ResourceMgr.h"

void Building::Initialize(const Vector3& translate) {
	// モデル
	mModel = std::make_unique<ModelInstance>();
	mModel->Create(ResourceMgr::GetInstance().GetModel("resources/building.gltf"));

	mTransform.mTranslate = translate;
	mTransform.Update();
}

void Building::Draw() {
	mModel->Draw(mTransform.GetWorldMat());
}
