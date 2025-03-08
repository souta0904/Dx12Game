#include "Buffer.h"
#include "DirectXBase.h"
#include "DirectXCommon.h"
#include <cassert>

// 定数バッファ
void ConstantBuffer::Create(uint32_t buffSize, void* initData) {
	mBuffSize = buffSize;
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = (mBuffSize + 0xFF) & ~0xFF;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Microsoft::WRL::ComPtr<ID3D12Device> device = DirectXBase::GetInstance().GetDevice();
	[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(&DirectXCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mBuff));
	assert(SUCCEEDED(hr));
	// リソースへのポインタを取得
	mBuff->Map(0, nullptr, &mData);
	if (initData) {
		// 初期化用のデータがあればコピーしておく
		Update(initData);
	}
}

void ConstantBuffer::Update(void* data) {
	memcpy(mData, data, mBuffSize);
}

void ConstantBuffer::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx) {
	cmdList->SetGraphicsRootConstantBufferView(idx, mBuff->GetGPUVirtualAddress());
}

// 頂点バッファ
void VertexBuffer::Create(uint32_t buffSize, uint32_t elemSize, void* initData) {
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = buffSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Microsoft::WRL::ComPtr<ID3D12Device> device = DirectXBase::GetInstance().GetDevice();
	[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(&DirectXCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mBuff));
	assert(SUCCEEDED(hr));
	// 頂点バッファビュー
	mVBV.BufferLocation = mBuff->GetGPUVirtualAddress();
	mVBV.SizeInBytes = buffSize;
	mVBV.StrideInBytes = elemSize;

	mBuff->Map(0, nullptr, &mData);
	if (initData) {
		Update(initData);
	}
}

void VertexBuffer::Update(void* data) {
	memcpy(mData, data, mVBV.SizeInBytes);
}

void VertexBuffer::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	cmdList->IASetVertexBuffers(0, 1, &mVBV);
}

// インデックスバッファ
void IndexBuffer::Create(uint32_t buffSize, void* initData) {
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = buffSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Microsoft::WRL::ComPtr<ID3D12Device> device = DirectXBase::GetInstance().GetDevice();
	[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(&DirectXCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mBuff));
	assert(SUCCEEDED(hr));
	// インデックスバッファビュー
	mIBV.BufferLocation = mBuff->GetGPUVirtualAddress();
	mIBV.SizeInBytes = buffSize;
	mIBV.Format = DXGI_FORMAT_R32_UINT;

	mBuff->Map(0, nullptr, &mData);
	if (initData) {
		Update(initData);
	}
}

void IndexBuffer::Update(void* data) {
	memcpy(mData, data, mIBV.SizeInBytes);
}

void IndexBuffer::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	cmdList->IASetIndexBuffer(&mIBV);
}

// 構造化バッファ
StructuredBuffer::~StructuredBuffer() {
	DirectXBase::GetInstance().GetSRVHeap()->Free(mSRV);
}

void StructuredBuffer::Create(uint32_t elemNum, uint32_t elemSize, void* initData) {
	mElemNum = elemNum;
	mElemSize = elemSize;
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = mElemNum * mElemSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	DirectXBase& directXBase = DirectXBase::GetInstance();
	Microsoft::WRL::ComPtr<ID3D12Device> device = directXBase.GetDevice();
	[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(&DirectXCommon::gHeapUpload, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mBuff));
	assert(SUCCEEDED(hr));

	mBuff->Map(0, nullptr, &mData);
	if (initData) {
		Update(initData);
	}

	// シェーダリソースビューを作成
	mSRV = directXBase.GetSRVHeap()->Allocate();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = mElemNum;
	srvDesc.Buffer.StructureByteStride = mElemSize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	device->CreateShaderResourceView(mBuff.Get(), &srvDesc, mSRV->mCPU);
}

void StructuredBuffer::Update(void* data) {
	memcpy(mData, data, mElemNum * mElemSize);
}

void StructuredBuffer::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx) {
	cmdList->SetGraphicsRootDescriptorTable(idx, mSRV->mGPU);
}
