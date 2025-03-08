#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

struct DescriptorHandle;

// 定数バッファ
class ConstantBuffer {
public:
	// 作成
	void Create(uint32_t buffSize, void* initData = nullptr);
	// 更新
	void Update(void* data);
	template <typename T>
	void Update(T& data) {
		Update(&data);
	}
	// セット
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx);
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff;
	uint32_t mBuffSize;
	void* mData;// リソースへのポインタ
};

// 頂点バッファ
class VertexBuffer {
public:
	void Create(uint32_t buffSize, uint32_t elemSize, void* initData = nullptr);
	void Update(void* data);
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff;
	D3D12_VERTEX_BUFFER_VIEW mVBV;
	void* mData;
};

// インデックスバッファ
class IndexBuffer {
public:
	void Create(uint32_t buffSize, void* initData = nullptr);
	void Update(void* data);
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff;
	D3D12_INDEX_BUFFER_VIEW mIBV;
	void* mData;
};

// 構造化バッファ
class StructuredBuffer {
public:
	~StructuredBuffer();
	void Create(uint32_t elemNum, uint32_t elemSize, void* initData = nullptr);
	void Update(void* data);
	template <typename T>
	void Update(T& data) {
		Update(&data);
	}
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx);
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff;
	uint32_t mElemNum;
	uint32_t mElemSize;
	void* mData;
	DescriptorHandle* mSRV;
};
