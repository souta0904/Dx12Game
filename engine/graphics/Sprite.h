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

class Sprite {
public:
	struct Vertex {
		Vector4 mPosition;
		Vector2 mUV;
	};
	struct Constant {
		Matrix4 mWVPMat;
		Matrix4 mUVMat;
		Vector4 mColor;
	};

	void Create(Texture* texture = nullptr);
	void Draw(const Matrix4& worldMat, Camera* camera = nullptr);
	Texture* GetTexture() const { return mTexture; }
	void SetTexture(Texture* texture);

private:
	void UpdateVB();
	void UpdateCB(const Matrix4& worldMat, const Matrix4& viewProjectionMat);

public:
	BlendMode mBlendMode = BlendMode::kNormal;
	Vector2 mPivot = Vector2(0.0f, 0.0f);
	bool mIsFlipX = false;
	bool mIsFlipY = false;
	Rect mDrawRc = { 0.0f, 0.0f, 0.0f, 0.0f };
	Vector2 mUVScale = Vector2(1.0f, 1.0f);
	float mUVRotate = 0.0f;
	Vector2 mUVTranslate = Vector2(0.0f, 0.0f);
	Vector4 mColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
private:
	Texture* mTexture;
	std::unique_ptr<VertexBuffer> mVB;
	std::unique_ptr<ConstantBuffer> mCB;
};
