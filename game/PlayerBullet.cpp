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
	mModel->Create(ResourceMgr::GetInstance().GetModel("resources/player.gltf"));
	// マテリアル
	mMaterial = std::make_unique<Material>();
	mMaterial->mColor = mModel->GetModel()->GetMaterial(0)->mColor;
	mMaterial->Create();
	mModel->SetMaterial(0, mMaterial.get());
	mModel->SetFlags(PSOFlags::kNormalBlend);

	mTransform = mPlayer->GetTransform();
	mType = ObjType::kPBullet;
	mCurrCourse = mPlayer->GetCurrCourse();
	mCoursePos = mPlayer->GetCoursePos();
	mCourseRot = mPlayer->GetCourseRot();
	mSpeed = 1.0f;
	SetRadius(1.0f);
}

void PlayerBullet::Update(InputBase*, float deltaTime) {
	mLife = MathUtil::Max(mLife - deltaTime, 0.0f);
	if (mLife <= 0.0f) {
		mIsDead = true;
	}
	Move(deltaTime);
	// マテリアル
	mMaterial->mColor.w = mAlpha;
	mMaterial->Update();
}

void PlayerBullet::Draw() {
	if (mAlpha > 0.0f) {
		mModel->Draw(mTransform.GetWorldMat());
	}
}

void PlayerBullet::OnCollision(CourseObj* obj) {
	if (obj->GetType() != ObjType::kPlayer) {
		mIsDead = true;
	}
}
