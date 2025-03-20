#pragma once

// ブレンドモード
enum class BlendMode {
	kNone,		// ブレンドなし
	kNormal,	// αブレンド
	kAddition,	// 加算
	kSubtract,	// 減算
	kMultiply,	// 乗算
	kScreen,	// スクリーン
	kMax
};
