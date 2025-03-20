#include "Player.h"
#include "Course.h"
#include "graphics/ModelBase.h"
#include "graphics/ResourceMgr.h"
#include "input/InputBase.h"

void Player::Initialize() {
	mModel = std::make_unique<ModelInstance>();
	mModel->Create(ResourceMgr::GetInstance().GetModel("resources/player.gltf"));
	//mTransform.mScale = Vector3(100.0f, 100.0f, 100.0f);
	//mModel->Create(ResourceMgr::GetInstance().GetModel("resources/walk.gltf"));
	//mModel->PlayAtName("Armature|mixamo.com|Layer0", 3.0f);
	//mModel->PlayAtIdx(0, 3.0f);
	mSpeed = 0.1f;
	mAngle = -MathUtil::kPiOver2;
	mAngleSpeed = MathUtil::kPiOver2;
}

void Player::Update(InputBase* input, float deltaTime) {
	if (mCurrCource) {
		mAngleVel = 0.0f;
		if (input->GetKey(DIK_A)) {
			mAngleVel -= mAngleSpeed * deltaTime;
		}
		if (input->GetKey(DIK_D)) {
			mAngleVel += mAngleSpeed * deltaTime;
		}
		mAngle += mAngleVel;

		mT += mSpeed * deltaTime;
		if (mT >= 1.0f) {// 次の区間へ
			if (mCurrSection < mCurrCource->GetSectionNum() - 1) {
				++mCurrSection;
				mT -= 1.0f;
			} else {
				mT = 1.0f;// 最後
			}
		}
		// リセット(デバッグ用)
		if (input->GetKeyDown(DIK_R)) {
			mCurrSection = 0;
			mT = 0.0f;
		}

		// 座標、回転
		Course::CircumferenceInfo courceInfo = mCurrCource->GetCircumferenceInfo(mCurrSection, mT, mAngle);
		mTransform.mTranslate = courceInfo.mPosition;
		mTransform.mRotate = Quaternion(Vector3::kUnitZ, mAngle + MathUtil::kPiOver2) * courceInfo.mRotate;
		// カメラ座標
		ModelBase::GetInstance().GetDefaultCamera()->mTranslate.x = mTransform.mTranslate.x / 2.0f;
		ModelBase::GetInstance().GetDefaultCamera()->mTranslate.y = mTransform.mTranslate.y / 2.0f + 2.5f;
		ModelBase::GetInstance().GetDefaultCamera()->mTranslate.z = mTransform.mTranslate.z - 10.0f;

		mTransform.Update();
		mModel->Update(deltaTime);
	}
}

void Player::Draw() {
	mModel->Draw(mTransform.GetWorldMat());
}
