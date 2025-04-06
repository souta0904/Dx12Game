#include "GameScene.h"
#include "Enemy.h"
#include "MathUtil.h"
#include "Player.h"

void GameScene::Initialize() {
	// コース1
	mCourse1 = std::make_unique<Course>(this);
	mCourse1->AddPoint({ Vector3(-5.0f,5.0f,100.0f),7.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,200.0f),7.0f });
	mCourse1->AddPoint({ Vector3(5.0f,0.0f,300.0f),7.0f });
	mCourse1->AddPoint({ Vector3(20.0f,0.0f,400.0f),5.0f });
	mCourse1->AddPoint({ Vector3(20.0f,0.0f,700.0f),5.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,1400.0f),7.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,1700.0f),7.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,1800.0f),7.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,1900.0f),7.0f });
	mCourse1->AddPoint({ Vector3(200.0f,5.0f,2000.0f),7.0f });
	mCourse1->AddPoint({ Vector3(50.0f,20.0f,2100.0f),7.0f });
	mCourse1->Create();
	// コース2
	mCourse2 = std::make_unique<Course>(this);
	mCourse2->AddPoint({ Vector3(100.0f,5.0f,100.0f),5.0f });
	mCourse2->AddPoint({ Vector3(100.0f,10.0f,200.0f),5.0f });
	mCourse2->AddPoint({ Vector3(100.0f,5.0f,300.0f),5.0f });
	mCourse2->AddPoint({ Vector3(100.0f,20.0f,400.0f),5.0f });
	mCourse2->Create();

	mCurrCourse = mCourse1.get();

	// プレイヤー
	std::unique_ptr<Player> player = std::make_unique<Player>(this);
	player->Initialize();
	mPlayer = player.get();
	AddObject(std::move(player));

	// エネミー
	for (uint32_t i = 0; i < 12; ++i) {
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(this);
		enemy->Initialize();
		enemy->SetHP(5);
		enemy->SetCurrCourse(mCourse1.get());
		enemy->SetCoursePos(1.0f);
		enemy->SetCourseRot(MathUtil::ToRadians(30.0f * i));
		AddObject(std::move(enemy));
	}
	for (uint32_t i = 0; i < 12; ++i) {
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(this);
		enemy->Initialize();
		enemy->SetHP(5);
		enemy->SetCurrCourse(mCourse1.get());
		enemy->SetCoursePos(1.5f);
		enemy->SetCourseRot(MathUtil::ToRadians(30.0f * i));
		AddObject(std::move(enemy));
	}
	for (uint32_t i = 0; i < 12; ++i) {
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(this);
		enemy->Initialize();
		enemy->SetHP(5);
		enemy->SetCurrCourse(mCourse1.get());
		enemy->SetCoursePos(2.0f);
		enemy->SetCourseRot(MathUtil::ToRadians(30.0f * i));
		AddObject(std::move(enemy));
	}
	for (uint32_t i = 0; i < 12; ++i) {
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(this);
		enemy->Initialize();
		enemy->SetHP(5);
		enemy->SetCurrCourse(mCourse1.get());
		enemy->SetCoursePos(2.25f);
		enemy->SetCourseRot(MathUtil::ToRadians(30.0f * i));
		AddObject(std::move(enemy));
	}
	for (uint32_t i = 0; i < 12; ++i) {
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(this);
		enemy->Initialize();
		enemy->SetHP(5);
		enemy->SetCurrCourse(mCourse1.get());
		enemy->SetCoursePos(2.5f);
		enemy->SetCourseRot(MathUtil::ToRadians(30.0f * i));
		AddObject(std::move(enemy));
	}
}

void GameScene::Terminate() {

}

void GameScene::Update(InputBase* input, float deltaTime) {
	mIsUpdating = true;
	for (auto& obj : mObjects) {
		obj->Update(input, deltaTime);
	}
	mIsUpdating = false;
	for (auto& obj : mWaitObjs) {
		mObjects.emplace_back(std::move(obj));
	}
	mWaitObjs.clear();

	// 当たり判定
	for (uint32_t i = 0; i < mObjects.size(); ++i) {
		for (uint32_t j = i + 1; j < mObjects.size(); ++j) {
			CourseObj* a = mObjects[i].get();
			CourseObj* b = mObjects[j].get();
			if (!a->GetIsDead() && !b->GetIsDead()) {
				if (Collision(a, b)) {
					a->OnCollision(b);
					b->OnCollision(a);
				}
			}
		}
	}

	// 削除
	mObjects.erase(
		std::remove_if(mObjects.begin(), mObjects.end(),
			[](const std::unique_ptr<CourseObj>& bullet) {
				return bullet->GetIsDead();
			}),
		mObjects.end()
	);
}

void GameScene::DrawBackground() {

}

void GameScene::DrawModel() {
	for (auto& obj : mObjects) {
		obj->Draw();
	}
}

void GameScene::DrawPrimitive() {
	mCourse1->DrawPrimitive();
	mCourse2->DrawPrimitive();
}

void GameScene::DrawForeground() {

}

void GameScene::AddObject(std::unique_ptr<CourseObj> obj) {
	if (mIsUpdating) {
		mWaitObjs.emplace_back(std::move(obj));
	} else {
		mObjects.emplace_back(std::move(obj));
	}
}
