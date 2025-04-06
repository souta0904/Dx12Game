#include "Enemy.h"
#include "graphics/ResourceMgr.h"
#include "MathUtil.h"

Enemy::Enemy(GameScene* gameScene)
	: CourseObj(gameScene) {

}

void Enemy::Initialize() {
	// モデル
	mModel = std::make_unique<ModelInstance>();
	mModel->Create(ResourceMgr::GetInstance().GetModel("resources/player.gltf"));
	// マテリアル
	mMaterial = std::make_unique<Material>();
	mMaterial->mColor = mModel->GetModel()->GetMaterial(0)->mColor;
	mMaterial->Create();
	mModel->SetMaterial(0, mMaterial.get());
	mModel->SetFlags(PSOFlags::kNormalBlend);

	mLocalRot = Quaternion(Vector3::kUnitY, MathUtil::kPi);// こっち向き
	mType = ObjType::kEnemy;
	mSpeed = -0.1f;
	SetRadius(1.0f);
}

void Enemy::Update(InputBase*, float deltaTime) {
	Move(deltaTime);
	// マテリアル
	mMaterial->mColor.w = mAlpha;
	mMaterial->Update();
}

void Enemy::Draw() {
	if (mAlpha > 0.0f) {
		mModel->Draw(mTransform.GetWorldMat());
	}
}

void Enemy::OnCollision(CourseObj* obj) {
	if (obj->GetType() == ObjType::kPBullet) {
		mIsDead = true;
	}
}
