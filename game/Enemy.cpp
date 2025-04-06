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
	mNormalCol = mModel->GetModel()->GetMaterial(0)->mColor;
	mMaterial->mColor = mNormalCol;
	mMaterial->Create();
	mModel->SetMaterial(0, mMaterial.get());
	mModel->SetFlags(PSOFlags::kNormalBlend);

	mLocalRot = Quaternion(Vector3::kUnitY, MathUtil::kPi);// こっち向き
	mType = ObjType::kEnemy;
	mSpeed = -0.1f;
	SetRadius(1.0f);

	mDamageCol = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
}

void Enemy::Update(InputBase*, float deltaTime) {
	Move(deltaTime);

	if (mDamageColTime <= 0.0f) {
		mMaterial->mColor = mNormalCol;
	} else {
		mDamageColTime = MathUtil::Max(mDamageColTime - deltaTime, 0.0f);
	}
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
	if (mAlpha > 0.0f) {
		if (obj->GetType() == ObjType::kPBullet) {
			--mHP;
			if (mHP <= 0) {
				mIsDead = true;
			}
			mMaterial->mColor = mDamageCol;
			mDamageColTime = 0.01f;
		}
	}
}
