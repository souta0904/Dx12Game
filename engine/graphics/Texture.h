#pragma once
#include "directX/DescriptorHeap.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

class Texture {
public:
	~Texture();
	bool Create(const std::string& path);
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx);
	const std::string& GetPath() const { return mPath; }
	Microsoft::WRL::ComPtr<ID3D12Resource> GetBuff() const { return mTexBuff; }
	DescriptorHandle* GetSRV() const { return mSRV; }
	uint32_t GetWidth() const { return mWidth; }
	uint32_t GetHeight() const { return mHeight; }
private:
	std::string mPath;
	Microsoft::WRL::ComPtr<ID3D12Resource> mTexBuff;
	DescriptorHandle* mSRV;
	uint32_t mWidth;
	uint32_t mHeight;
};
