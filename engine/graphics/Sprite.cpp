#include "Sprite.h"
#include "Camera.h"
#include "directX/DirectXBase.h"
#include "SpriteBase.h"
#include "Texture.h"

void Sprite::Create(Texture* texture) {
	SetTexture(texture);
	mVB = std::make_unique<VertexBuffer>();
	mVB->Create(sizeof(Vertex) * 4, sizeof(Vertex));
	mCB = std::make_unique<ConstantBuffer>();
	mCB->Create(sizeof(Constant));
}

void Sprite::Draw(const Matrix4& worldMat, Camera* camera) {
	if (mTexture) {
		SpriteBase& spriteBase = SpriteBase::GetInstance();
		UpdateVB();
		if (!camera) {
			camera = spriteBase.GetDefaultCamera();
		}
		UpdateCB(worldMat, camera->GetViewProjectionMat());
		spriteBase.SetPipelineState(mBlendMode);
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = DirectXBase::GetInstance().GetCmdList();
		mVB->Set(cmdList);
		mCB->Set(cmdList, 0);
		mTexture->Set(cmdList, 1);
		cmdList->DrawInstanced(4, 1, 0, 0);
	}
}

void Sprite::SetTexture(Texture* texture) {
	mTexture = texture;
	if (mTexture) {
		mDrawRc.left = 0.0f;
		mDrawRc.top = 0.0f;
		mDrawRc.right = static_cast<float>(mTexture->GetWidth());
		mDrawRc.bottom = static_cast<float>(mTexture->GetHeight());
	}
}

void Sprite::UpdateVB() {
	Vertex data[4] = {};
	float w = mDrawRc.right - mDrawRc.left;
	float h = mDrawRc.bottom - mDrawRc.top;
	float l = (0.0f - mPivot.x) * w;
	float t = (0.0f - mPivot.y) * h;
	float r = (1.0f - mPivot.x) * w;
	float b = (1.0f - mPivot.y) * h;
	if (mIsFlipX) {
		l = -l;
		r = -r;
	}
	if (mIsFlipY) {
		t = -t;
		b = -b;
	}
	data[0].mPosition = Vector4(l, t, 0.0f, 1.0f);
	data[1].mPosition = Vector4(r, t, 0.0f, 1.0f);
	data[2].mPosition = Vector4(l, b, 0.0f, 1.0f);
	data[3].mPosition = Vector4(r, b, 0.0f, 1.0f);
	w = static_cast<float>(mTexture->GetWidth());
	h = static_cast<float>(mTexture->GetHeight());
	l = mDrawRc.left / w;
	t = mDrawRc.top / h;
	r = mDrawRc.right / w;
	b = mDrawRc.bottom / h;
	data[0].mUV = Vector2(l, t);
	data[1].mUV = Vector2(r, t);
	data[2].mUV = Vector2(l, b);
	data[3].mUV = Vector2(r, b);
	mVB->Update(data);
}

void Sprite::UpdateCB(const Matrix4& worldMat, const Matrix4& viewProjectionMat) {
	Constant data = {};
	data.mWVPMat = worldMat * viewProjectionMat;
	data.mUVMat = CreateAffine4(
		Vector3(mUVScale.x, mUVScale.y, 1.0f),
		Quaternion(Vector3(0.0f, 0.0f, 1.0f), mUVRotate),
		Vector3(mUVTranslate.x, mUVTranslate.y, 0.0f)
	);
	data.mColor = mColor;
	mCB->Update(data);
}
