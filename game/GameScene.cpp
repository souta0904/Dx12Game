#include "GameScene.h"
#include "Enemy.h"
#include "ImGuiWrapper.h"
#include "input/InputBase.h"
#include "MathUtil.h"
#include "Player.h"
#include <format>

void GameScene::Initialize() {
	// コース1
	mCourse1 = std::make_unique<Course>(this);
	mCourse1->AddPoint({ Vector3(-5.0f,5.0f,100.0f),7.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,200.0f),7.0f });
	mCourse1->AddPoint({ Vector3(5.0f,15.0f,300.0f),7.0f });
	mCourse1->AddPoint({ Vector3(20.0f,15.0f,400.0f),5.0f });
	mCourse1->AddPoint({ Vector3(20.0f,15.0f,700.0f),5.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,900.0f),7.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,1000.0f),7.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,1100.0f),7.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,1200.0f),7.0f });
	mCourse1->AddPoint({ Vector3(200.0f,5.0f,1300.0f),7.0f });
	mCourse1->AddPoint({ Vector3(50.0f,20.0f,1400.0f),7.0f });
	mCourse1->Create();
	// コース2
	mCourse2 = std::make_unique<Course>(this);
	mCourse2->AddPoint({ Vector3(100.0f,5.0f,100.0f),5.0f });
	mCourse2->AddPoint({ Vector3(100.0f,10.0f,200.0f),5.0f });
	mCourse2->AddPoint({ Vector3(100.0f,5.0f,300.0f),5.0f });
	mCourse2->AddPoint({ Vector3(100.0f,20.0f,400.0f),5.0f });
	mCourse2->Create();

	mCurrCourse = mCourse1.get();

	Load();
}

void GameScene::Terminate() {
	Unload();
}

void GameScene::Update(InputBase* input, float deltaTime) {
	if (input->GetKeyDown(DIK_R) || input->GetPadButtonDown(XINPUT_GAMEPAD_START)) {
		Unload();
		Load();
	}

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

	ImGui::Begin("Game scene");
	ImGui::Text(std::format("Object count: {}", mObjects.size()).c_str());
	ImGui::End();
}

void GameScene::DrawBackground() {

}

void GameScene::DrawModel() {
	for (auto& obj : mObjects) {
		obj->Draw();
	}
	for (auto& building : mBuildings) {
		building->Draw();
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

void GameScene::Load() {
	// プレイヤー
	std::unique_ptr<Player> player = std::make_unique<Player>(this);
	player->Initialize();
	mPlayer = player.get();
	AddObject(std::move(player));

	// エネミー
	for (uint32_t i = 0; i < 12; ++i) {
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(this);
		enemy->Initialize();
		enemy->SetHP(2);
		enemy->SetCurrCourse(mCourse1.get());
		enemy->SetCoursePos(1.0f);
		enemy->SetCourseRot(MathUtil::ToRadians(30.0f * i));
		AddObject(std::move(enemy));
	}
	for (uint32_t i = 0; i < 12; ++i) {
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(this);
		enemy->Initialize();
		enemy->SetHP(2);
		enemy->SetCurrCourse(mCourse1.get());
		enemy->SetCoursePos(1.5f);
		enemy->SetCourseRot(MathUtil::ToRadians(30.0f * i));
		AddObject(std::move(enemy));
	}
	for (uint32_t i = 0; i < 12; ++i) {
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(this);
		enemy->Initialize();
		enemy->SetHP(2);
		enemy->SetCurrCourse(mCourse1.get());
		enemy->SetCoursePos(2.0f);
		enemy->SetCourseRot(MathUtil::ToRadians(30.0f * i));
		AddObject(std::move(enemy));
	}
	for (uint32_t i = 0; i < 12; ++i) {
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(this);
		enemy->Initialize();
		enemy->SetHP(2);
		enemy->SetCurrCourse(mCourse1.get());
		enemy->SetCoursePos(2.25f);
		enemy->SetCourseRot(MathUtil::ToRadians(30.0f * i));
		AddObject(std::move(enemy));
	}
	for (uint32_t i = 0; i < 12; ++i) {
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(this);
		enemy->Initialize();
		enemy->SetHP(2);
		enemy->SetCurrCourse(mCourse1.get());
		enemy->SetCoursePos(2.5f);
		enemy->SetCourseRot(MathUtil::ToRadians(30.0f * i));
		AddObject(std::move(enemy));
	}

	std::vector<Vector3> pos;
	//
	pos.emplace_back(Vector3(-17.0f, 0.0f, 110.0f));
	pos.emplace_back(Vector3(-19.0f, 0.0f, 125.0f));
	pos.emplace_back(Vector3(-17.0f, 0.0f, 140.0f));
	pos.emplace_back(Vector3(-15.0f, 0.0f, 155.0f));
	pos.emplace_back(Vector3(-15.0f, 0.0f, 170.0f));
	pos.emplace_back(Vector3(-15.0f, 0.0f, 185.0f));
	//
	pos.emplace_back(Vector3(-14.0f, 0.0f, 210.0f));
	pos.emplace_back(Vector3(-16.0f, 0.0f, 225.0f));
	pos.emplace_back(Vector3(-16.0f, 0.0f, 240.0f));
	pos.emplace_back(Vector3(-14.0f, 0.0f, 255.0f));
	pos.emplace_back(Vector3(-12.0f, 0.0f, 270.0f));
	pos.emplace_back(Vector3(-14.0f, 0.0f, 285.0f));

	//
	pos.emplace_back(Vector3(13.0f, 0.0f, 110.0f));
	pos.emplace_back(Vector3(11.0f, 0.0f, 125.0f));
	pos.emplace_back(Vector3(13.0f, 0.0f, 140.0f));
	pos.emplace_back(Vector3(15.0f, 0.0f, 155.0f));
	pos.emplace_back(Vector3(15.0f, 0.0f, 170.0f));
	pos.emplace_back(Vector3(15.0f, 0.0f, 185.0f));
	//
	pos.emplace_back(Vector3(18.0f, 0.0f, 210.0f));
	pos.emplace_back(Vector3(16.0f, 0.0f, 225.0f));
	pos.emplace_back(Vector3(16.0f, 0.0f, 240.0f));
	pos.emplace_back(Vector3(18.0f, 0.0f, 255.0f));
	pos.emplace_back(Vector3(20.0f, 0.0f, 270.0f));
	pos.emplace_back(Vector3(18.0f, 0.0f, 285.0f));

	for (uint32_t i = 0; i < pos.size(); ++i) {
		std::unique_ptr<Building> b = std::make_unique<Building>();
		b->Initialize(pos[i]);
		mBuildings.emplace_back(std::move(b));
	}
}

void GameScene::Unload() {
	mObjects.clear();
	mWaitObjs.clear();
	mBuildings.clear();
}
