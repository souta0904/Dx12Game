#include "Game.h"
#include "Helper.h"
#include "Window.h"
#include <Windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	Helper::Log("Hello,Dx12Game!\n");

	Game game;
	Window::GetInstance().Create(1920, 1080, "Game title");// ウィンドウの作成
	game.Run();// ゲームの実行

	return 0;
}
