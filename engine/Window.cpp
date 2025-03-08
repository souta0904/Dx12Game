#include "Window.h"
#include "Helper.h"
#include <ImGui/imgui_impl_win32.h>

// ImGuiのウィンドウプロシージャの宣言
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ウィンドウプロシージャ
LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
		return true;
	}
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::Create(uint32_t width, uint32_t height, const std::string& title) {
	mClientWidth = width;
	mClientHeight = height;
	mTitle = title;

	// ウィンドウクラスの設定、登録
	mWndClass.cbSize = sizeof(WNDCLASSEX);
	mWndClass.lpfnWndProc = WndProc;
	mWndClass.hInstance = GetModuleHandle(nullptr);
	mWndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	mWndClass.lpszClassName = L"RdWndClass";
	RegisterClassEx(&mWndClass);
	// クライアントサイズからウィンドウサイズを求める
	RECT wndRc = { 0,0,static_cast<LONG>(mClientWidth),static_cast<LONG>(mClientHeight) };
	AdjustWindowRect(&wndRc, WS_OVERLAPPEDWINDOW, false);
	std::wstring wtitle = Helper::ConvertString(mTitle);
	// ウィンドウの作成、表示
	mHWnd = CreateWindow(
		mWndClass.lpszClassName,
		wtitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wndRc.right - wndRc.left,
		wndRc.bottom - wndRc.top,
		nullptr,
		nullptr,
		mWndClass.hInstance,
		nullptr
	);
	ShowWindow(mHWnd, SW_SHOW);

	Helper::Log("Create window.\n");
}

bool Window::ProcessMessage() {
	MSG msg = {};
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.message == WM_QUIT;
}
