#pragma once
#include <d3d12.h>
#include <vector>
#include <wrl.h>

// デスクリプタハンドル
struct DescriptorHandle {
	D3D12_CPU_DESCRIPTOR_HANDLE mCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE mGPU;
};

// デスクリプタヒープ
class DescriptorHeap {
public:
	void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t num, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);

	// デスクリプタハンドル割り当て、解放
	DescriptorHandle* Allocate();
	void Free(DescriptorHandle*& descHandle);
	// アクティブなデスクリプタの数
	uint32_t GetActiveNum() const;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const { return mDescriptorHeap; }

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
	// 先頭ハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE mCPUStart;
	D3D12_GPU_DESCRIPTOR_HANDLE mGPUStart;
	// デスクリプタハンドルのインクリメントサイズ
	uint32_t mIncrementSize;

	// デスクリプタハンドルの管理
	// アクティブフラグとデスクリプタハンドルのペア
	std::vector<std::pair<bool, DescriptorHandle>> mDescriptorHandles;
	uint32_t mNextIdx;// リングバッファ実装用
};
