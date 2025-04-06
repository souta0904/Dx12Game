#include "Player.h"
#include "Course.h"
#include "GameScene.h"
#include "graphics/ModelBase.h"
#include "graphics/ResourceMgr.h"
#include "ImGuiWrapper.h"
#include "input/InputBase.h"
#include <format>

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
	mCoursePos = -mT2Cam;
	mCourseRot = -MathUtil::kPiOver2;
	mSpeed = 0.1f;

	mRotVel = 0.0f;
	mRotSpeed = MathUtil::kPiOver2;

	// カメラ
	Course::CenterInfo centerInfo = mCurrCourse->GetCenterInfo(0.0f);
	ModelCamera* camera = ModelBase::GetInstance().GetDefaultCamera();
	camera->mTranslate = centerInfo.mPosition;
	camera->mRotate = centerInfo.mRotate;
	camera->mFovY = MathUtil::kPiOver180 * 50.0f;// 60度
}

void Player::Update(InputBase* input, float deltaTime) {
	// 回転
	mRotVel = 0.0f;
	Vector2 ls = input->GetLeftStick();
	if (input->GetKey(DIK_A) || ls.x <= -0.5f) {
		mRotVel -= mRotSpeed;
	}
	if (input->GetKey(DIK_D) || ls.x >= 0.5f) {
		mRotVel += mRotSpeed;
	}
	// 弾
	//mCooldown = MathUtil::Max(mCooldown - deltaTime, 0.0f);
	mCooldown -= deltaTime;
	if ((input->GetKey(DIK_SPACE) || input->GetPadButton(XINPUT_GAMEPAD_A)) && mCooldown <= 0.0f) {
		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>(mGameScene);
		bullet->Initialize();
		mGameScene->AddObject(std::move(bullet));
		static const float kInitCooldown = 0.05f;
		mCooldown = kInitCooldown;
	}

	float addSpeed = mSpeed * deltaTime;
	mCoursePos += addSpeed;
	float sectionNum = static_cast<float>(mCurrCourse->GetSectionNum());
	if (mCoursePos > sectionNum) {
		mCoursePos = sectionNum;
	}
	mCourseRot += mRotVel * deltaTime;
	Course::AroundInfo aroundInfo = mCurrCourse->GetAroundInfo(mCoursePos, mCourseRot, Course::kDistFromAround);
	mTransform.mTranslate = aroundInfo.mPosition;
	mTransform.mRotate = Quaternion(Vector3::kUnitZ, mCourseRot) * aroundInfo.mRotate;
	mTransform.Update();
	SetCenter(mTransform.mTranslate);
	mModel->Update(deltaTime);

	// カメラ
	float cameraT = mCoursePos + mT2Cam;
	Course::CenterInfo centerInfo = mCurrCourse->GetCenterInfo(cameraT);
	ModelCamera* camera = ModelBase::GetInstance().GetDefaultCamera();
	//camera->mTranslate = MathUtil::Lerp(camera->mTranslate, centerInfo.mPosition, 0.05f);// ガタガタする
	camera->mTranslate = centerInfo.mPosition;
	camera->mRotate = Slerp(camera->mRotate, centerInfo.mRotate, 0.1f);

	ImGui::Begin("Player");
	ImGui::Text(std::format("Add speed: {}", addSpeed).c_str());
	ImGui::Text(std::format("Player t: {}", mCoursePos).c_str());
	ImGui::Text(std::format("Camera t: {}", cameraT).c_str());
	ImGui::End();
}

void Player::Draw() {
	mModel->Draw(mTransform.GetWorldMat());
}
