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
	mModel->Create(ResourceMgr::GetInstance().GetModel("resources/player.gltf"));

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
	Course::AroundInfo aroundInfo = mCurrCourse->GetAroundInfo(mCurrT, mCourseRot);
	mTransform.mTranslate = aroundInfo.mPosition;
	mTransform.mRotate = Quaternion(Vector3::kUnitZ, mCourseRot + MathUtil::kPiOver2) * aroundInfo.mRotate;

	mTransform.Update();
}

void PlayerBullet::Draw() {
	mModel->Draw(mTransform.GetWorldMat());
}
