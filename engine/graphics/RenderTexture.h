#pragma once
#include "directX/DescriptorHeap.h"
#include "Vector4.h"
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

// レンダーターゲット
class RenderTexture {
public:
	void Create(uint32_t width, uint32_t height, bool useDepth);
	void Begin(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
	void End(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
	// レンダーターゲットをシェーダリソース(テクスチャ)としてセット
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx);

private:
	// 幅と高さ
	uint32_t mWidth;
	uint32_t mHeight;
	// 深度の有無
	bool mUseDepth;

	// クリア色
	Vector4 mClearColor;
	// レンダーターゲット
	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff;
	DescriptorHandle* mRTV;
	DescriptorHandle* mSRV;
	D3D12_VIEWPORT mViewport;// ビューポート
	D3D12_RECT mScissorRc;// シザー矩形
	// 深度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthBuff;
	DescriptorHandle* mDSV;
};
