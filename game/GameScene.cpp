#include "GameScene.h"

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

	// プレイヤー
	mPlayer = std::make_unique<Player>(this);
	mPlayer->Initialize();

	// 敵
	std::unique_ptr<Enemy> enemy1 = std::make_unique<Enemy>(this);
	enemy1->Initialize(mCourse1.get(), 5.0f, 0.0f);
	mEnemies.push_back(std::move(enemy1));
}

void GameScene::Terminate() {

}

void GameScene::Update(InputBase* input, float deltaTime) {
	mPlayer->Update(input, deltaTime);
	for (auto& enemy : mEnemies) {
		enemy->Update(input, deltaTime);
	}

	for (auto& pb : mPlayer->GetBullets()) {
		for (auto& e : mEnemies) {
			if (Collision(pb.get(), e.get())) {
				pb->OnCollision();
				e->OnCollision();
			}
		}
	}
}

void GameScene::DrawBackground() {

}

void GameScene::DrawModel() {
	mPlayer->Draw();
	for (auto& enemy : mEnemies) {
		enemy->Draw();
	}
}

void GameScene::DrawPrimitive() {
	mCourse1->DrawPrimitive();
	mCourse2->DrawPrimitive();
}

void GameScene::DrawForeground() {

}
