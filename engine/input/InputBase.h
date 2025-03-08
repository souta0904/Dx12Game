#pragma once
#include "Vector2.h"
#include <cstdint>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Windows.h>
#include <wrl.h>
#include <Xinput.h>

// 入力
class InputBase {
public:
	void Initialize();
	void Terminate();
	// 更新(毎フレーム最初に呼び出す)
	void Update();

	// キーボード
	bool GetKey(uint8_t keyCode) const;// 押している
	bool GetKeyUp(uint8_t keyCode) const;// 離した
	bool GetKeyDown(uint8_t keyCode) const;// 押した
	// マウス
	bool GetMouseButton(uint8_t button) const;// 押している
	bool GetMouseButtonUp(uint8_t button) const;// 離した
	bool GetMouseButtonDown(uint8_t button) const;// 押した
	const Vector2& GetMouseMovement() const { return mMouseMovement; }
	float GetMouseWheel() const { return mMouseWheel; }
	const Vector2& GetMousePosition() const { return mMousePosition; }
	// パッド
	bool IsPadConnected() const { return mIsPadConnected; }
	bool GetPadButton(int button) const;// 押している
	bool GetPadButtonUp(int button) const;// 離した
	bool GetPadButtonDown(int button) const;// 押した
	const Vector2& GetLeftStick() const { return mLeftStick; }
	const Vector2& GetRightStick() const { return mRightStick; }
	float GetLeftTrigger() const { return mLeftTrigger; }
	float GetRightTrigger() const { return mRightTrigger; }
	void SetLeftStickDeadZone(float deadZone) { mLeftStickDeadZone = deadZone; }
	void SetRightStickDeadZone(float deadZone) { mRightStickDeadZone = deadZone; }
	void SetTriggerDeadZone(float deadZone) { mTriggerDeadZone = deadZone; }

private:
	// スティックを正規化(-1.0から1.0)
	Vector2 NormalizeStick(uint16_t inputX, uint16_t inputY, float deadZone);
	// トリガーを正規化(0.0から1.0)
	float NormalizeTrigger(uint8_t input, float deadZone);

private:
	HWND mHWnd;
	Microsoft::WRL::ComPtr<IDirectInput8> mDirectInput;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mKeyboardDevice;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mMouseDevice;

	// キーボード
	uint8_t mCurrKeys[256];
	uint8_t mPrevKeys[256];
	// マウス
	BYTE mCurrMouseButtons[8];
	BYTE mPrevMouseButtons[8];
	Vector2 mMouseMovement;// 移動量
	float mMouseWheel;// ホイール
	Vector2 mMousePosition;// 座標
	// パッド
	bool mIsPadConnected;// 接続されているか
	WORD mCurrPadButton;
	WORD mPrevPadButton;
	Vector2 mLeftStick;// 左スティック
	Vector2 mRightStick;// 右スティック
	float mLeftTrigger;// 左トリガー
	float mRightTrigger;// 右トリガー
	// デッドゾーン
	float mLeftStickDeadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	float mRightStickDeadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
	float mTriggerDeadZone = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
};
