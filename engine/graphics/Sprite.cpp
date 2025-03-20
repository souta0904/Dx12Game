#include "Sprite.h"
#include "Camera.h"
#include "directX/DirectXBase.h"
#include "SpriteBase.h"
#include "Texture.h"

void Sprite::Create(Texture* texture) {
	// テクスチャをセット
	SetTexture(texture);
	// 頂点バッファを作成
	mVB = std::make_unique<VertexBuffer>();
	mVB->Create(sizeof(Vertex) * 4, sizeof(Vertex));
	// 定数バッファを作成
	mCB = std::make_unique<ConstantBuffer>();
	mCB->Create(sizeof(Constant));
}

void Sprite::Draw(const Matrix4& worldMat, Camera* camera) {
	if (mTexture) {
		SpriteBase& spriteBase = SpriteBase::GetInstance();

		// 頂点バッファを更新
		UpdateVB();
		// 定数バッファを更新
		if (!camera) {
			camera = spriteBase.GetDefaultCamera();
		}
		UpdateCB(worldMat, camera->GetViewProjectionMat());

		// 描画コマンドを積む
		spriteBase.SetPipelineState(mBlendMode);
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = DirectXBase::GetInstance().GetCmdList();
		mVB->Set(cmdList);
		mCB->Set(cmdList, static_cast<uint32_t>(SpriteBase::RootParamNum::kConstant));
		mTexture->Set(cmdList, static_cast<uint32_t>(SpriteBase::RootParamNum::kTexture));
		cmdList->DrawInstanced(4, 1, 0, 0);// 描画
	}
}

void Sprite::SetTexture(Texture* texture) {
	mTexture = texture;
	// テクスチャの領域もセット
	if (mTexture) {
		mDrawRc.left = 0.0f;
		mDrawRc.top = 0.0f;
		mDrawRc.right = static_cast<float>(mTexture->GetWidth());
		mDrawRc.bottom = static_cast<float>(mTexture->GetHeight());
	} else {
		mDrawRc.left = 0.0f;
		mDrawRc.top = 0.0f;
		mDrawRc.right = 0.0f;
		mDrawRc.bottom = 0.0f;
	}
}

// 頂点バッファを更新
void Sprite::UpdateVB() {
	Vertex data[4] = {};
	// 座標
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
	// UV座標
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
	// 更新
	mVB->Update(data);
}

// 定数バッファを更新
void Sprite::UpdateCB(const Matrix4& worldMat, const Matrix4& vpMat) {
	Constant data = {};
	// ワールドビュープロジェクション行列
	data.mWVPMat = worldMat * vpMat;
	// UV変換行列
	data.mUVMat = CreateAffine4(
		Vector3(mUVScale.x, mUVScale.y, 1.0f),
		Quaternion(Vector3(0.0f, 0.0f, 1.0f), mUVRotate),
		Vector3(mUVTranslate.x, mUVTranslate.y, 0.0f)
	);
	// 色
	data.mColor = mColor;
	// 更新
	mCB->Update(data);
}
