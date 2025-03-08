#pragma once
#include "directX/DescriptorHeap.h"
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

// レンダーターゲットテクスチャ
class RenderTexture {
public:
	void Create(uint32_t width, uint32_t height, bool useDepth);
	void Begin(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
	void End(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx);
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mTexBuff;
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthBuff;
	DescriptorHandle* mRTV;
	DescriptorHandle* mDSV;
	DescriptorHandle* mSRV;
	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRc;
};
