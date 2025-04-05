#include "PlayerBullet.h"
#include "Course.h"
#include "graphics/ResourceMgr.h"
#include "MathUtil.h"
#include "Player.h"

PlayerBullet::PlayerBullet(Player* player)
	: mPlayer(player) {

}

void PlayerBullet::Initialize() {
	mModel = std::make_unique<ModelInstance>();
	mModel->Create(ResourceMgr::GetInstance().GetModel("resources/bullet.gltf"));
	mMaterial = std::make_unique<Material>();
	mMaterial->mColor = mModel->GetModel()->GetMaterial(0)->mColor;// 色
	mMaterial->Create();
	mModel->SetMaterial(0, mMaterial.get());
	mModel->SetFlags(PSOFlags::kNormalBlend);// 透過する

	mTransform = mPlayer->GetTransform();
	mCurrCourse = mPlayer->GetCurrCourse();
	mCurrT = mPlayer->GetCurrT();
	mCourseRot = mPlayer->GetCourseRot();
}

void PlayerBullet::Update(InputBase*, float deltaTime) {
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

	mTransform.Update();

	// 透明度の計算
	float pt = mPlayer->GetCurrT();
	mMaterial->mColor.w = Course::CalcTransparent(pt, mCurrT);
	mMaterial->Update();
}

void PlayerBullet::Draw() {
	if (mMaterial->mColor.w > 0.0f) {
		mModel->Draw(mTransform.GetWorldMat());
	}
}
