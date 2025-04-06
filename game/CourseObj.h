#pragma once
#include "CircleCollider.h"
#include "graphics/Model.h"
#include "Transform.h"

class GameScene;
class InputBase;
class Player;
class Course;

// 種類
enum class ObjType {
	kNone = 0,
	kPlayer = 1,
	kEnemy = 2,
	kPBullet = 3,	// Player bullet
	kEBullet = 4,	// Enemy bullet
	kBlock = 5,
	kHBlock = 6		// Hard block
};

// コース上のオブジェクト
class CourseObj : public CircleCollider {
public:
	CourseObj(GameScene* gameScene);
	virtual ~CourseObj() {}
	virtual void Initialize() = 0;
	virtual void Update(InputBase* input, float deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void OnCollision(CourseObj* obj) = 0;

	bool GetIsDead() const { return mIsDead; }
	const Transform& GetTransform() const { return mTransform; }
	ObjType GetType() const { return mType; }
	Course* GetCurrCourse() const { return mCurrCourse; }
	float GetCoursePos() const { return mCoursePos; }
	float GetCourseRot() const { return mCourseRot; }
	float GetSpeed() const { return mSpeed; }
	float GetAlpha() const { return mAlpha; }
	void SetCurrCourse(Course* course) { mCurrCourse = course; }
	void SetCoursePos(float coursePos) { mCoursePos = coursePos; }
	void SetCourseRot(float courseRot) { mCourseRot = courseRot; }
	void SetSpeed(float speed) { mSpeed = speed; }
	void SetAlpha(float alpha) { mAlpha = alpha; }

protected:
	// 更新(プレイヤー以外)
	void Move(float deltaTime);

protected:
	GameScene* mGameScene;
	Player* mPlayer;
	bool mIsDead = false;
	Quaternion mLocalRot;
	Transform mTransform;
	ObjType mType = ObjType::kNone;
	// コース
	Course* mCurrCourse;
	float mCoursePos;
	float mCourseRot;
	float mSpeed;
	float mAlpha;// 透過
};
