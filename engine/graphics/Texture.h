#pragma once
#include "directX/DescriptorHeap.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

// テクスチャ
class Texture {
public:
	~Texture();
	bool Create(const std::string& path);
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx);

	const std::string& GetPath() const { return mPath; }
	uint32_t GetWidth() const { return mWidth; }
	uint32_t GetHeight() const { return mHeight; }
	Microsoft::WRL::ComPtr<ID3D12Resource> GetBuff() const { return mBuff; }
	DescriptorHandle* GetSRV() const { return mSRV; }
private:
	// パス
	std::string mPath;
	// 幅
	uint32_t mWidth;
	// 高さ
	uint32_t mHeight;
	// バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff;
	// SRVデスクリプタハンドル
	DescriptorHandle* mSRV;
};
