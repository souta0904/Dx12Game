#include "Texture.h"
#include "directX/DirectXBase.h"
#include "directX/DirectXCommon.h"
#include "Helper.h"
#include <DirectXTex/d3dx12.h>
#include <DirectXTex/DirectXTex.h>
#include <format>

Texture::~Texture() {
	// デスクリプタハンドルを解放
	DirectXBase::GetInstance().GetSRVHeap()->Free(mSRV);
}

bool Texture::Create(const std::string& path) {
	mPath = path;
	// テクスチャを読み込む
	DirectX::ScratchImage scratchImage = {};
	std::wstring wpath = Helper::ConvertString(mPath);
	HRESULT hr = DirectX::LoadFromWICFile(wpath.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, scratchImage);
	if (FAILED(hr)) {
		return false;
	}

	// ミップマップを生成
	DirectX::ScratchImage mipChain = {};
	hr = GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipChain);
	if (FAILED(hr)) {
		return false;
	}

	// 幅と高さ
	DirectX::TexMetadata metadata = mipChain.GetMetadata();
	mWidth = static_cast<uint32_t>(metadata.width);
	mHeight = static_cast<uint32_t>(metadata.height);

	// テクスチャバッファをコピー先として作成
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
	desc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
	desc.Format = metadata.format;
	desc.SampleDesc.Count = 1;
	DirectXBase& directXBase = DirectXBase::GetInstance();
	Microsoft::WRL::ComPtr<ID3D12Device> device = directXBase.GetDevice();
	hr = device->CreateCommittedResource(&DirectXCommon::gHeapDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&mBuff));
	if (FAILED(hr)) {
		return false;
	}

	// アップロード用の中間リソースを作成
	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device.Get(), mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), subresource);
	D3D12_RESOURCE_DESC intermediateDesc = {};
	intermediateDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	intermediateDesc.Width = GetRequiredIntermediateSize(mBuff.Get(), 0, static_cast<UINT>(subresource.size()));
	intermediateDesc.Height = 1;
	intermediateDesc.DepthOrArraySize = 1;
	intermediateDesc.MipLevels = 1;
	intermediateDesc.SampleDesc.Count = 1;
	intermediateDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateBuff = nullptr;
	hr = device->CreateCommittedResource(&DirectXCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &intermediateDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&intermediateBuff));
	if (FAILED(hr)) {
		return false;
	}

	// アップロードのコマンドを積む
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = directXBase.GetCmdList();
	UpdateSubresources(cmdList.Get(), mBuff.Get(), intermediateBuff.Get(), 0, 0, static_cast<UINT>(subresource.size()), subresource.data());
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mBuff.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	cmdList->ResourceBarrier(1, &barrier);
	// アップロード
	directXBase.ExecuteCmdList(false);

	// デスクリプタハンドルを割り当て
	mSRV = directXBase.GetSRVHeap()->Allocate();
	// シェーダリソースビューを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DirectX::MakeSRGB(metadata.format);
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);
	device->CreateShaderResourceView(mBuff.Get(), &srvDesc, mSRV->mCPU);

	Helper::Log(std::format("Create '{}'\n", mPath));
	return true;
}

void Texture::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx) {
	cmdList->SetGraphicsRootDescriptorTable(idx, mSRV->mGPU);
}
