#pragma once
#include "DescriptorHeap.h"
#include <array>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <memory>
#include <string>
#include <wrl.h>

// DirectX基盤
class DirectXBase final {
public:
	// シングルトンを実装
	static DirectXBase& GetInstance() {
		static DirectXBase instance;
		return instance;
	}
	DirectXBase(const DirectXBase&) = delete;
	DirectXBase& operator=(const DirectXBase&) = delete;
	DirectXBase(DirectXBase&&) = delete;
	DirectXBase& operator=(DirectXBase&&) = delete;
private:
	DirectXBase() = default;
	~DirectXBase();

public:
	// リークチェック
	struct LeakChecker {
#ifdef _DEBUG
		~LeakChecker() {
			Microsoft::WRL::ComPtr<IDXGIDebug1> debug = nullptr;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
				debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			}
		}
#endif
	};

	void Initialize();
	void Terminate();
	void BeginFrame();
	// 最終レンダーターゲットをセット
	void SetFinalRT();
	// コマンドリストを実行
	void ExecuteCmdList(bool isFlip = true, UINT syncInterval = 1);

	// アクセサ
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const { return mDevice; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCmdList() const { return mCmdList; }
	DescriptorHeap* GetRTVHeap() const { return mRTVHeap.get(); }
	DescriptorHeap* GetDSVHeap() const { return mDSVHeap.get(); }
	DescriptorHeap* GetSRVHeap() const { return mSRVHeap.get(); }

private:
	LeakChecker mLeakChecker;// リークチェック
	Microsoft::WRL::ComPtr<IDXGIFactory7> mFactory;// DXGIファクトリ
	Microsoft::WRL::ComPtr<ID3D12Device> mDevice;// デバイス
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCmdQueue;// コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCmdAllocator;// コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCmdList;// コマンドリスト
	Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;// スワップチェーン
	std::unique_ptr<DescriptorHeap> mRTVHeap;// RTV用デスクリプタヒープ
	std::unique_ptr<DescriptorHeap> mDSVHeap;// DSV用デスクリプタヒープ
	std::unique_ptr<DescriptorHeap> mSRVHeap;// SRV用デスクリプタヒープ
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> mBackBuffs;// バックバッファ
	std::array<DescriptorHandle*, 2> mRTVs;// レンダーターゲットビュー
	UINT64 mFenceVal;// フェンス値
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;// フェンス
	HANDLE mFenceEvent;// フェンスイベント
	D3D12_VIEWPORT mViewport;// ビューポート
	D3D12_RECT mScissorRc;// シザー矩形

	// 現在のバックバッファのインデックス
	UINT mBackBuffIdx;
};
