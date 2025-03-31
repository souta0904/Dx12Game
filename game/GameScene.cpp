#include "GameScene.h"

void GameScene::Initialize() {
	// コース1
	mCourse1 = std::make_unique<Course>(this);
	mCourse1->AddPoint({ Vector3(-5.0f,5.0f,100.0f),5.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,200.0f),5.0f });
	mCourse1->AddPoint({ Vector3(5.0f,0.0f,300.0f),5.0f });
	mCourse1->AddPoint({ Vector3(20.0f,0.0f,400.0f),2.0f });
	mCourse1->AddPoint({ Vector3(-5.0f,-5.0f,500.0f),2.0f });
	mCourse1->AddPoint({ Vector3(0.0f,20.0f,600.0f),5.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,700.0f),5.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,800.0f),5.0f });
	mCourse1->AddPoint({ Vector3(0.0f,10.0f,900.0f),5.0f });
	mCourse1->AddPoint({ Vector3(200.0f,5.0f,1000.0f),5.0f });
	mCourse1->AddPoint({ Vector3(50.0f,20.0f,1100.0f),5.0f });
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
}

void GameScene::Terminate() {

}

void GameScene::Update(InputBase* input, float deltaTime) {
	mPlayer->Update(input, deltaTime);
}

void GameScene::DrawBackground() {

}

void GameScene::DrawModel() {
	mPlayer->Draw();
}

void GameScene::DrawPrimitive() {
	mCourse1->DrawPrimitive();
	mCourse2->DrawPrimitive();
}

void GameScene::DrawForeground() {

}
