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
	mSpeed = 0.25f;
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

		// カメラ
		float cameraT = mT - 0.1f;
		uint32_t cameraSection = mCurrSection;
		if (cameraT <= 0.0f) {
			if (cameraSection > 0) {
				cameraT += 1.0f;
				--cameraSection;
			} else {
				cameraT = 0.0f;
				cameraSection = 0;
			}
		}
		Course::CenterInfo centerInfo = mCurrCource->GetCenterInfo(cameraSection, cameraT);
		float nextT = cameraT += 0.25f;
		if (nextT >= 1.0f) {
			if (cameraSection < mCurrCource->GetSectionNum() - 1) {
				nextT -= 1.0f;
				// 次の区間
				++cameraSection;
			} else {
				nextT = 1.0f;
			}
		}
		Course::CenterInfo nextInfo = mCurrCource->GetCenterInfo(cameraSection, nextT);
		Quaternion rotate = Course::CalcDirection(centerInfo.mPosition, nextInfo.mPosition);
		ModelCamera* camera = ModelBase::GetInstance().GetDefaultCamera();
		camera->mTranslate = centerInfo.mPosition;
		camera->mRotate = rotate;

		mTransform.Update();
		mModel->Update(deltaTime);
	}
}

void Player::Draw() {
	mModel->Draw(mTransform.GetWorldMat());
}
