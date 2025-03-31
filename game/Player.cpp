#include "Player.h"
#include "Course.h"
#include "GameScene.h"
#include "graphics/ModelBase.h"
#include "graphics/ResourceMgr.h"
#include "input/InputBase.h"

Player::Player(GameScene* gameScene)
	: mGameScene(gameScene) {

}

void Player::Initialize() {
	mModel = std::make_unique<ModelInstance>();
	mModel->Create(ResourceMgr::GetInstance().GetModel("resources/player.gltf"));
	//mTransform.mScale = Vector3(100.0f, 100.0f, 100.0f);
	//mModel->Create(ResourceMgr::GetInstance().GetModel("resources/walk.gltf"));
	//mModel->PlayAtName("Armature|mixamo.com|Layer0", 3.0f);
	//mModel->PlayAtIdx(0, 3.0f);

	mCurrCourse = mGameScene->GetCourse1();
	mCurrT = 0.0f;

	mSpeed = 0.5f;
	mCourseRot = -MathUtil::kPiOver2;
	mRotVel = 0.0f;
	mRotSpeed = MathUtil::kPiOver2;

	// カメラ
	Course::CenterInfo centerInfo = mCurrCourse->GetCenterInfo(0.0f);
	ModelCamera* camera = ModelBase::GetInstance().GetDefaultCamera();
	camera->mTranslate = centerInfo.mPosition;
	camera->mRotate = centerInfo.mRotate;
}

void Player::Update(InputBase* input, float deltaTime) {
	if (mCurrCourse) {

		// 入力
		mRotVel = 0.0f;
		if (input->GetKey(DIK_A)) {
			mRotVel -= mRotSpeed;
		}
		if (input->GetKey(DIK_D)) {
			mRotVel += mRotSpeed;
		}
		// リセット
		if (input->GetKeyDown(DIK_R)) {
			mCurrT = 0.0f;
		}
		// コース変更
		if (input->GetKeyDown(DIK_1)) {
			mCurrCourse = mGameScene->GetCourse1();
			mCurrT = 0.0f;
		}
		if (input->GetKeyDown(DIK_2)) {
			mCurrCourse = mGameScene->GetCourse2();
			mCurrT = 0.0f;
		}

		// 更新
		mCurrT += mSpeed * deltaTime;
		if (mCurrT > mCurrCourse->GetSectionNum()) {
			mCurrT = static_cast<float>(mCurrCourse->GetSectionNum());
		}
		mCourseRot += mRotVel * deltaTime;

		// 座標と回転
		Course::AroundInfo aroundInfo = mCurrCourse->GetAroundInfo(mCurrT, mCourseRot);
		mTransform.mTranslate = aroundInfo.mPosition;
		mTransform.mRotate = Quaternion(Vector3::kUnitZ, mCourseRot + MathUtil::kPiOver2) * aroundInfo.mRotate;

		// カメラ
		float cameraT = mCurrT - 0.1f;
		Course::CenterInfo centerInfo = mCurrCourse->GetCenterInfo(cameraT);
		ModelCamera* camera = ModelBase::GetInstance().GetDefaultCamera();
		camera->mTranslate = MathUtil::Lerp(camera->mTranslate, centerInfo.mPosition, 0.1f);
		camera->mRotate = Slerp(camera->mRotate, centerInfo.mRotate, 0.1f);

		mTransform.Update();
		mModel->Update(deltaTime);

	}
}

void Player::Draw() {
	mModel->Draw(mTransform.GetWorldMat());
}
