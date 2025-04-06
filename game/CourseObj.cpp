#include "CourseObj.h"
#include "Course.h"
#include "GameScene.h"
#include "MathUtil.h"
#include "Player.h"

CourseObj::CourseObj(GameScene* gameScene)
	: mGameScene(gameScene) {
	mPlayer = mGameScene->GetPlayer();
}

void CourseObj::Move(float deltaTime) {
	mCoursePos += mSpeed * deltaTime;
	if (mCoursePos > mCurrCourse->GetSectionNum() ||
		mCoursePos <= 0.0f) {
		mIsDead = true;
	}
	Course::AroundInfo aroundInfo = mCurrCourse->GetAroundInfo(mCoursePos, mCourseRot, Course::kDistFromAround);
	mTransform.mTranslate = aroundInfo.mPosition;
	mTransform.mRotate = mLocalRot * Quaternion(Vector3::kUnitZ, mCourseRot) * aroundInfo.mRotate;
	mAlpha = Course::CalcTransparent(mPlayer->GetCoursePos(), mCoursePos);
	mTransform.Update();
	SetCenter(mTransform.mTranslate);
}
