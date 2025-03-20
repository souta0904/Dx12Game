#pragma once
#include "BlendMode.h"
#include "directX/Buffer.h"
#include "Matrix4.h"
#include "Rect.h"
#include "Vector2.h"
#include "Vector4.h"
#include <memory>

class Texture;
class Camera;

// スプライト
class Sprite {
public:
	// 頂点
	struct Vertex {
		Vector4 mPosition;
		Vector2 mUV;
	};
	// 定数
	struct Constant {
		Matrix4 mWVPMat;
		Matrix4 mUVMat;
		Vector4 mColor;
	};

	void Create(Texture* texture = nullptr);
	void Draw(const Matrix4& worldMat, Camera* camera = nullptr);

	Texture* GetTexture() const { return mTexture; }
	void SetTexture(Texture* texture);// cpp

private:
	// バッファを更新
	void UpdateVB();
	void UpdateCB(const Matrix4& worldMat, const Matrix4& vpMat);

private:
	Texture* mTexture;// テクスチャ
	std::unique_ptr<VertexBuffer> mVB;// 頂点バッファ
	std::unique_ptr<ConstantBuffer> mCB;// 定数バッファ

public:
	// ブレンドモード
	BlendMode mBlendMode = BlendMode::kNormal;
	// スプライトの中心点(0.0から1.0)
	Vector2 mPivot = Vector2(0.0f, 0.0f);
	// 反転フラグ
	bool mIsFlipX = false;
	bool mIsFlipY = false;
	// テクスチャの領域
	Rect mDrawRc = { 0.0f, 0.0f, 0.0f, 0.0f };
	// UVトランスフォーム
	Vector2 mUVScale = Vector2(1.0f, 1.0f);
	float mUVRotate = 0.0f;
	Vector2 mUVTranslate = Vector2(0.0f, 0.0f);
	// 色
	Vector4 mColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
};
