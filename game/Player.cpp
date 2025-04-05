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

	mSpeed = 0.3f;
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
		// 弾を発射
		mCooldown = MathUtil::Max(mCooldown - deltaTime, 0.0f);
		if (input->GetKey(DIK_SPACE) && mCooldown <= 0.0f) {
			std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>(this);
			bullet->Initialize();
			mBullets.emplace_back(std::move(bullet));
			mCooldown = 0.05f;
		}

		// 更新
		mCurrT += mSpeed * deltaTime;
		if (mCurrT > mCurrCourse->GetSectionNum()) {
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
		camera->mFovY = MathUtil::kPiOver180 * 60.0f;

		mTransform.Update();
		mModel->Update(deltaTime);

		for (auto& bullet : mBullets) {
			bullet->Update(input, deltaTime);
		}

		// 死んだ弾を削除
		mBullets.erase(
			std::remove_if(mBullets.begin(), mBullets.end(),
				[](const std::unique_ptr<PlayerBullet>& bullet) {
					return bullet->GetIsDead();
				}),
			mBullets.end()
		);

	}
}

void Player::Draw() {
	mModel->Draw(mTransform.GetWorldMat());

	for (auto& bullet : mBullets) {
		bullet->Draw();
	}
}
