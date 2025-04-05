#include "Player.h"
#include "Course.h"
#include "GameScene.h"
#include "graphics/ModelBase.h"
#include "graphics/ResourceMgr.h"
#include "input/InputBase.h"

Player::Player(GameScene* gameScene)
	: CourseObj(gameScene) {

}

void Player::Initialize() {
	mModel = std::make_unique<ModelInstance>();
	mModel->Create(ResourceMgr::GetInstance().GetModel("resources/player.gltf"));
	/*
	// walk用
	mTransform.mScale = Vector3(100.0f, 100.0f, 100.0f);
	mModel->Create(ResourceMgr::GetInstance().GetModel("resources/walk.gltf"));
	mModel->PlayAtName("Armature|mixamo.com|Layer0", 3.0f);
	mModel->PlayAtIdx(0, 3.0f);
	*/

	mType = ObjType::kPlayer;
	mCurrCourse = mGameScene->GetCurrCourse();
	mCurrT = 0.0f;
	mCourseRot = -MathUtil::kPiOver2;
	mTSpeed = 0.3f;
	mRotVel = 0.0f;
	mRotSpeed = MathUtil::kPiOver2;

	// カメラ
	Course::CenterInfo centerInfo = mCurrCourse->GetCenterInfo(0.0f);
	ModelCamera* camera = ModelBase::GetInstance().GetDefaultCamera();
	camera->mTranslate = centerInfo.mPosition;
	camera->mRotate = centerInfo.mRotate;
	camera->mFovY = MathUtil::kPiOver180 * 60.0f;// 60度
}

void Player::Update(InputBase* input, float deltaTime) {
	if (!mIsDead && mCurrCourse) {
		// 回転
		mRotVel = 0.0f;
		if (input->GetKey(DIK_A)) {
			mRotVel -= mRotSpeed;
		}
		if (input->GetKey(DIK_D)) {
			mRotVel += mRotSpeed;
		}
		// 弾
		mCooldown = MathUtil::Max(mCooldown - deltaTime, 0.0f);
		if (input->GetKey(DIK_SPACE) && mCooldown <= 0.0f) {
			std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>(mGameScene);
			bullet->Initialize();
			mGameScene->AddObject(std::move(bullet));
			mCooldown = 0.05f;
		}

		mCurrT += mTSpeed * deltaTime;
		if (mCurrT > mCurrCourse->GetSectionNum()) {// 終わり
			mCurrT = static_cast<float>(mCurrCourse->GetSectionNum());
		}
		mCourseRot += mRotVel * deltaTime;

		// 座標と回転
		Course::AroundInfo aroundInfo = mCurrCourse->GetAroundInfo(mCurrT, mCourseRot, Course::kDistFromAround);
		mTransform.mTranslate = aroundInfo.mPosition;
		mTransform.mRotate = Quaternion(Vector3::kUnitZ, mCourseRot + MathUtil::kPiOver2) * aroundInfo.mRotate;

		// カメラ
		float cameraT = mCurrT + mT2Cam;
		Course::CenterInfo centerInfo = mCurrCourse->GetCenterInfo(cameraT);
		ModelCamera* camera = ModelBase::GetInstance().GetDefaultCamera();
		camera->mTranslate = MathUtil::Lerp(camera->mTranslate, centerInfo.mPosition, 0.05f);
		camera->mRotate = Slerp(camera->mRotate, centerInfo.mRotate, 0.05f);

		mTransform.Update();
		mModel->Update(deltaTime);
	}
}

void Player::Draw() {
	if (!mIsDead) {
		mModel->Draw(mTransform.GetWorldMat());
	}
}
