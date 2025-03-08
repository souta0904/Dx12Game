#include "DescriptorHeap.h"
#include "DirectXBase.h"
#include <cassert>

void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num, D3D12_DESCRIPTOR_HEAP_FLAGS flags) {
	// デスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = type;
	desc.NumDescriptors = num;
	desc.Flags = flags;
	Microsoft::WRL::ComPtr<ID3D12Device> device = DirectXBase::GetInstance().GetDevice();
	[[maybe_unused]] HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mDescriptorHeap));
	assert(SUCCEEDED(hr));
	// デスクリプタハンドルの先頭を取得
	mCPUStart = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	if (flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
		mGPUStart = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}
	// デスクリプタハンドルのインクリメントサイズを取得
	mIncrementSize = device->GetDescriptorHandleIncrementSize(desc.Type);

	mDescriptorHandles.resize(desc.NumDescriptors);
}

void DescriptorHeap::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	ID3D12DescriptorHeap* dh[] = { mDescriptorHeap.Get() };
	cmdList->SetDescriptorHeaps(1, dh);
}

// 割り当て
DescriptorHandle* DescriptorHeap::Allocate() {
	uint32_t size = static_cast<uint32_t>(mDescriptorHandles.size());
	for (uint32_t i = 0; i < size; ++i) {
		uint32_t idx = (mNextIdx + i) % size;
		if (!mDescriptorHandles[idx].first) {
			mDescriptorHandles[idx].first = true;

			// デスクリプタハンドルを計算
			mDescriptorHandles[idx].second.mCPU = mCPUStart;
			mDescriptorHandles[idx].second.mCPU.ptr += mIncrementSize * idx;
			mDescriptorHandles[idx].second.mGPU = mGPUStart;
			mDescriptorHandles[idx].second.mGPU.ptr += mIncrementSize * idx;

			mNextIdx = (idx + 1) % size;
			return &mDescriptorHandles[idx].second;
		}
	}
	return nullptr;
}

// 解放
void DescriptorHeap::Free(DescriptorHandle*& descHandle) {
	if (descHandle) {
		for (auto& dh : mDescriptorHandles) {
			if (&dh.second == descHandle) {
				dh.first = false;
				break;
			}
		}
		descHandle = nullptr;
	}
}

uint32_t DescriptorHeap::GetActiveNum() const {
	uint32_t num = 0;
	for (auto& dh : mDescriptorHandles) {
		if (dh.first) {
			++num;
		}
	}
	return num;
}
