#include "Enemy.h"
#include "Course.h"
#include "GameScene.h"
#include "graphics/ResourceMgr.h"
#include "MathUtil.h"

Enemy::Enemy(GameScene* gameScene)
	: mGameScene(gameScene) {

}

void Enemy::Initialize(Course* course, float t, float rot) {
	mCurrCourse = course;
	mCurrT = t;
	mCourseRot = rot;

	mModel = std::make_unique<ModelInstance>();
	mModel->Create(ResourceMgr::GetInstance().GetModel("resources/bullet.gltf"));
	// マテリアル作成
	mMaterial = std::make_unique<Material>();
	mMaterial->mColor = mModel->GetModel()->GetMaterial(0)->mColor;// 色
	mMaterial->Create();
	mModel->SetMaterial(0, mMaterial.get());
	mModel->SetFlags(PSOFlags::kNormalBlend);// 透過する

	SetRadius(1.0f);
}

void Enemy::Update(InputBase*, float deltaTime) {
	mCurrT += mSpeed * deltaTime;
	if (mCurrT > mCurrCourse->GetSectionNum()) {
		mIsDead = true;
	}

	// 座標と回転
	Course::AroundInfo aroundInfo = mCurrCourse->GetAroundInfo(mCurrT, mCourseRot, Course::kDistFromAround);
	mTransform.mTranslate = aroundInfo.mPosition;
	mTransform.mRotate = Quaternion(Vector3::kUnitZ, mCourseRot + MathUtil::kPiOver2) * aroundInfo.mRotate;

	mTransform.Update();
	SetCenter(mTransform.mTranslate);

	// 透明度の計算
	float pt = mGameScene->GetPlayer()->GetCurrT();
	mMaterial->mColor.w = Course::CalcTransparent(pt, mCurrT);
	mMaterial->Update();
}

void Enemy::Draw() {
	if (mMaterial->mColor.w > 0.0f && !mIsDead) {
		mModel->Draw(mTransform.GetWorldMat());
	}
}
