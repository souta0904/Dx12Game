#include "Player.h"
#include "Course.h"
#include "graphics/ModelBase.h"
#include "graphics/ResourceMgr.h"
#include "input/InputBase.h"

void Player::Initialize() {
	mModel = std::make_unique<Model>();
	mModel->Create(ResourceMgr::GetInstance().GetModelData("resources/walk.gltf"));
	mModel->Play("Armature|mixamo.com|Layer0", 3.0f);
	mTransform.mScale = Vector3(100.0f, 100.0f, 100.0f);
	mSpeed = 0.2f;
}

void Player::Update(InputBase* input, float deltaTime) {
	mRadian = 0.0f;
	if (input->GetKey(DIK_A)) {
		mRadian -= 3.14f / 5.0f;
	}
	if (input->GetKey(DIK_D)) {
		mRadian += 3.14f / 5.0f;
	}
	// 進む
	mT += mSpeed * deltaTime;
	if (mT >= 1.0f) {
		if (mSection < mCource->GetSectionNum() - 1) {
			++mSection;
			// 次の区間
			mT -= 1.0f;
		} else {
			mT = 1.0f;
		}
	}
	if (input->GetKeyDown(DIK_R)) {
		mSection = 0;
		mT = 0.0f;
	}
	Course::Info p = mCource->GetInfoAtT(mSection, mT);
	mAngle += mRadian * deltaTime;
	mTransform.mTranslate = Vector3(
		p.mRadius * cosf(mAngle),
		p.mRadius * sinf(mAngle),
		0.0f) +
		p.mPosition;
	mTransform.mRotate = Quaternion(Vector3::kUnitX, -MathUtil::kPiOver2) * Quaternion(Vector3::kUnitZ, mAngle + MathUtil::kPiOver2) * p.mRotate;
	ModelBase::GetInstance().GetDefaultCamera()->mTranslate = p.mPosition - Vector3(0.0f, 0.0f, 6.0f);
	mModel->Update(deltaTime);
}

void Player::Draw() {
	Matrix4 affine = CreateAffine4(mTransform.mScale, mTransform.mRotate, mTransform.mTranslate);
	mModel->Draw(affine);
}
