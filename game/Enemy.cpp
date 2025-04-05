#include "Enemy.h"
#include "Course.h"
#include "GameScene.h"
#include "graphics/ResourceMgr.h"
#include "MathUtil.h"
#include "Player.h"

Enemy::Enemy(GameScene* gameScene)
	: CourseObj(gameScene) {

}

void Enemy::Initialize() {
	mPlayer = mGameScene->GetPlayer();
	// モデル
	mModel = std::make_unique<ModelInstance>();
	mModel->Create(ResourceMgr::GetInstance().GetModel("resources/bullet.gltf"));
	// マテリアル
	mMaterial = std::make_unique<Material>();
	mMaterial->mColor = mModel->GetModel()->GetMaterial(0)->mColor;
	mMaterial->Create();
	mModel->SetMaterial(0, mMaterial.get());
	mModel->SetFlags(PSOFlags::kNormalBlend);

	mType = ObjType::kEnemy;
	SetRadius(1.0f);
}

void Enemy::Update(InputBase*, float deltaTime) {
	if (!mIsDead) {
		mCurrT += mSpeed * deltaTime;
		// コース外で死亡
		if (mCurrT > mCurrCourse->GetSectionNum() ||
			mCurrT <= 0.0f) {
			mIsDead = true;
		}

		// 座標と回転
		Course::AroundInfo aroundInfo = mCurrCourse->GetAroundInfo(mCurrT, mCourseRot, Course::kDistFromAround);
		mTransform.mTranslate = aroundInfo.mPosition;
		mTransform.mRotate = Quaternion(Vector3::kUnitY, MathUtil::kPi) * Quaternion(Vector3::kUnitZ, mCourseRot + MathUtil::kPiOver2) * aroundInfo.mRotate;

		// マテリアル
		float pt = mPlayer->GetCurrT();
		mMaterial->mColor.w = Course::CalcTransparent(pt, mCurrT);
		mMaterial->Update();

		mTransform.Update();
		SetCenter(mTransform.mTranslate);
	}
}

void Enemy::Draw() {
	if (!mIsDead) {
		if (mMaterial->mColor.w > 0.0f) {// 範囲外
			mModel->Draw(mTransform.GetWorldMat());
		}
	}
}

void Enemy::OnCollision(CourseObj* obj) {
	// プレイヤーの弾であれば死亡
	if (obj->GetType() == ObjType::kPBullet) {
		mIsDead = true;
	}
}
