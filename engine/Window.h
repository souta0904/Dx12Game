#pragma once
#include <cstdint>
#include <string>
#include <Windows.h>

// ウィンドウ
class Window final {
public:
	// シングルトンを実装
	static Window& GetInstance() {
		static Window instance;
		return instance;
	}
	// コピー、代入、ムーブ、ムーブ代入の禁止
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;
private:
	// コンストラクタ、デストラクタを隠ぺい
	Window() = default;
	~Window() = default;

public:
	// ウィンドウプロシージャ
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	// ウィンドウの作成
	void Create(uint32_t width, uint32_t height, const std::string& title);
	// メッセージの処理
	// ループ終了でtrueを返す
	bool ProcessMessage();

	// アクセサ
	uint32_t GetClientWidth() const { return mClientWidth; }
	uint32_t GetClientHeight() const { return mClientHeight; }
	const WNDCLASSEX& GetWndClass() const { return mWndClass; }
	HWND GetHWnd() const { return mHWnd; }

private:
	// クライアントサイズ
	uint32_t mClientWidth;
	uint32_t mClientHeight;
	// タイトル
	std::string mTitle;
	// ウィンドウクラス
	WNDCLASSEX mWndClass;
	// ウィンドウハンドル
	HWND mHWnd = nullptr;
};
