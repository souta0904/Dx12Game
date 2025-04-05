#include "PlayerBullet.h"
#include "Course.h"
#include "GameScene.h"
#include "graphics/ResourceMgr.h"
#include "MathUtil.h"
#include "Player.h"

PlayerBullet::PlayerBullet(GameScene* gameScene)
	: CourseObj(gameScene) {

}

void PlayerBullet::Initialize() {
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

	mTransform = mPlayer->GetTransform();
	mType = ObjType::kPBullet;
	mCurrCourse = mPlayer->GetCurrCourse();
	mCurrT = mPlayer->GetCurrT();
	mCourseRot = mPlayer->GetCourseRot();
	SetRadius(1.0f);
}

void PlayerBullet::Update(InputBase*, float deltaTime) {
	if (!mIsDead) {
		mLife = MathUtil::Max(mLife - deltaTime, 0.0f);
		if (mLife <= 0.0f) {
			mIsDead = true;
		}

		mCurrT += mSpeed * deltaTime;
		if (mCurrT > mCurrCourse->GetSectionNum()) {
			mIsDead = true;
		}

		// 座標と回転
		Course::AroundInfo aroundInfo = mCurrCourse->GetAroundInfo(mCurrT, mCourseRot, Course::kDistFromAround);
		mTransform.mTranslate = aroundInfo.mPosition;
		mTransform.mRotate = Quaternion(Vector3::kUnitZ, mCourseRot + MathUtil::kPiOver2) * aroundInfo.mRotate;

		// マテリアル
		float pt = mPlayer->GetCurrT();
		mMaterial->mColor.w = Course::CalcTransparent(pt, mCurrT);
		mMaterial->Update();

		mTransform.Update();
		SetCenter(mTransform.mTranslate);
	}
}

void PlayerBullet::Draw() {
	if (!mIsDead) {
		if (mMaterial->mColor.w > 0.0f) {
			mModel->Draw(mTransform.GetWorldMat());
		}
	}
}

void PlayerBullet::OnCollision(CourseObj* obj) {
	// プレイヤー以外であれば死亡
	if (obj->GetType() != ObjType::kPlayer) {
		mIsDead = true;
	}
}
