#pragma once
#include "CircleCollider.h"
#include "graphics/Model.h"
#include "Transform.h"

class GameScene;
class InputBase;
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
	CourseObj(GameScene* gameScene) : mGameScene(gameScene) {}
	virtual ~CourseObj() {}
	virtual void Initialize() = 0;
	virtual void Update(InputBase* input, float deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void OnCollision(CourseObj* obj) = 0;

	bool GetIsDead() const { return mIsDead; }
	const Transform& GetTransform() const { return mTransform; }
	ObjType GetType() const { return mType; }
	Course* GetCurrCourse() const { return mCurrCourse; }
	float GetCurrT() const { return mCurrT; }
	float GetCourseRot() const { return mCourseRot; }
	void SetCurrCourse(Course* course) { mCurrCourse = course; }
	void SetCurrT(float t) { mCurrT = t; }
	void SetCourseRot(float courseRot) { mCourseRot = courseRot; }

protected:
	GameScene* mGameScene;
	bool mIsDead = false;
	Transform mTransform;
	ObjType mType = ObjType::kNone;
	// コース
	Course* mCurrCourse;
	float mCurrT;
	float mCourseRot;
};
