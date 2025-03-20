#include "InputBase.h"
#include "MathUtil.h"
#include "Window.h"
#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")

void InputBase::Initialize() {
	Window& window = Window::GetInstance();
	mHWnd = window.GetHWnd();
	HRESULT hr = DirectInput8Create(window.GetWndClass().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, &mDirectInput, nullptr);
	assert(SUCCEEDED(hr));

	// キーボードの初期化
	hr = mDirectInput->CreateDevice(GUID_SysKeyboard, &mKeyboardDevice, nullptr);
	assert(SUCCEEDED(hr));
	hr = mKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	hr = mKeyboardDevice->SetCooperativeLevel(mHWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	// マウスの初期化
	hr = mDirectInput->CreateDevice(GUID_SysMouse, &mMouseDevice, nullptr);
	assert(SUCCEEDED(hr));
	hr = mMouseDevice->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(hr));
	hr = mMouseDevice->SetCooperativeLevel(mHWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(hr));

	mKeyboardDevice->Acquire();
	mMouseDevice->Acquire();
}

void InputBase::Terminate() {
	mMouseDevice->Unacquire();
	mKeyboardDevice->Unacquire();
}

void InputBase::Update() {
	// キーボード
	// 前のフレームを保存
	memcpy(mPrevKeys, mCurrKeys, sizeof(mCurrKeys));
	ZeroMemory(mCurrKeys, sizeof(mCurrKeys));
	// キーボードの状態を取得
	HRESULT hr = mKeyboardDevice->GetDeviceState(sizeof(mCurrKeys), mCurrKeys);
	if (FAILED(hr)) {
		// 失敗したら再取得
		mKeyboardDevice->Acquire();
		mKeyboardDevice->GetDeviceState(sizeof(mCurrKeys), mCurrKeys);
	}

	// マウス
	memcpy(mPrevMouseButtons, mCurrMouseButtons, sizeof(mCurrMouseButtons));
	DIMOUSESTATE2 mouseState = {};
	hr = mMouseDevice->GetDeviceState(sizeof(mouseState), &mouseState);
	if (FAILED(hr)) {
		mMouseDevice->Acquire();
		mMouseDevice->GetDeviceState(sizeof(mouseState), &mouseState);
	}
	memcpy(mCurrMouseButtons, &mouseState.rgbButtons, sizeof(mCurrMouseButtons));
	mMouseMovement = Vector2(static_cast<float>(mouseState.lX), static_cast<float>(mouseState.lY));// 移動量
	mMouseWheel = static_cast<float>(mouseState.lZ);// ホイール
	// 座標
	POINT mousePosition = {};
	GetCursorPos(&mousePosition);
	ScreenToClient(mHWnd, &mousePosition);// クライアント座標へ
	mMousePosition = Vector2(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));

	// パッド
	mPrevPadButton = mCurrPadButton;
	XINPUT_STATE xinputState = {};
	DWORD result = XInputGetState(0, &xinputState);
	if (result == ERROR_SUCCESS) {
		mIsPadConnected = true;
		mCurrPadButton = xinputState.Gamepad.wButtons;
		// スティックとトリガーを正規化
		mLeftStick = NormalizeStick(xinputState.Gamepad.sThumbLX, xinputState.Gamepad.sThumbLY, mLeftStickDeadZone);
		mRightStick = NormalizeStick(xinputState.Gamepad.sThumbRX, xinputState.Gamepad.sThumbRY, mRightStickDeadZone);
		mLeftTrigger = NormalizeTrigger(xinputState.Gamepad.bLeftTrigger, mTriggerDeadZone);
		mRightTrigger = NormalizeTrigger(xinputState.Gamepad.bRightTrigger, mTriggerDeadZone);
	} else {
		mIsPadConnected = false;
	}
}

// スティックを正規化
Vector2 InputBase::NormalizeStick(uint16_t inputX, uint16_t inputY, float deadZone) {
	const float kMax = 32767.0f;
	float len = std::sqrtf(static_cast<float>(inputX * inputX + inputY * inputY));
	Vector2 normalizedStick;
	if (len > deadZone) {
		float normalizedLen = (len - deadZone) / (kMax - deadZone);
		normalizedLen = MathUtil::Clamp(normalizedLen, 0.0f, 1.0f);
		normalizedStick.x = inputX * normalizedLen / len;
		normalizedStick.y = inputY * normalizedLen / len;
	}
	return normalizedStick;
}

// トリガーを正規化
float InputBase::NormalizeTrigger(uint8_t input, float deadZone) {
	const float kMax = 255.0f;
	float normalize = 0.0f;
	if (input > deadZone) {
		normalize = static_cast<float>(input - deadZone) / (kMax - deadZone);
	}
	return normalize;
}

bool InputBase::GetKey(uint8_t keyCode) const {
	assert(keyCode >= 0 && keyCode < _countof(mCurrKeys));
	return mCurrKeys[keyCode] & 0x80;
}

bool InputBase::GetKeyUp(uint8_t keyCode) const {
	assert(keyCode >= 0 && keyCode < _countof(mCurrKeys));
	return !(mCurrKeys[keyCode] & 0x80) && mPrevKeys[keyCode] & 0x80;
}

bool InputBase::GetKeyDown(uint8_t keyCode) const {
	assert(keyCode >= 0 && keyCode < _countof(mCurrKeys));
	return mCurrKeys[keyCode] & 0x80 && !(mPrevKeys[keyCode] & 0x80);
}

bool InputBase::GetMouseButton(uint8_t button) const {
	assert(button >= 0 && button < _countof(mCurrMouseButtons));
	return mCurrMouseButtons[button] & 0x80;
}

bool InputBase::GetMouseButtonUp(uint8_t button) const {
	assert(button >= 0 && button < _countof(mCurrMouseButtons));
	return mCurrMouseButtons[button] & 0x80 && !(mPrevMouseButtons[button] & 0x80);
}

bool InputBase::GetMouseButtonDown(uint8_t button) const {
	assert(button >= 0 && button < _countof(mCurrMouseButtons));
	return !(mCurrMouseButtons[button] & 0x80) && mPrevMouseButtons[button] & 0x80;
}

bool InputBase::GetPadButton(int button) const {
	return mCurrPadButton & button;
}

bool InputBase::GetPadButtonUp(int button) const {
	return !(mCurrPadButton & button) && mPrevPadButton & button;
}

bool InputBase::GetPadButtonDown(int button) const {
	return mCurrPadButton & button && !(mPrevPadButton & button);
}
