#include "DirectXBase.h"
#include "DirectXCommon.h"
#include "Helper.h"
#include "Window.h"
#include <cassert>
#include <format>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

void DirectXBase::Initialize() {
	DirectXCommon::Initialize();

#ifdef _DEBUG
	// デバッグレイヤーを有効化
	Microsoft::WRL::ComPtr<ID3D12Debug1> debug = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
		debug->EnableDebugLayer();
		debug->SetEnableGPUBasedValidation(true);
	}
#endif

	// DXGIファクトリの作成
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&mFactory));
	assert(SUCCEEDED(hr));

	// 性能がいいアダプタを選ぶ
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	for (UINT i = 0;
		mFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND;
		++i) {
		DXGI_ADAPTER_DESC3 desc = {};
		hr = useAdapter->GetDesc3(&desc);
		assert(SUCCEEDED(hr));
		// ソフトウェアアダプタは使わない
		if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Helper::Log(std::format(L"Use adapter: {}\n", desc.Description));
			break;
		}
	}
	assert(useAdapter);

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};
	const char* str[] = { "12.2","12.1","12.0" };
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// デバイスの作成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&mDevice));
		if (SUCCEEDED(hr)) {
			Helper::Log(std::format("Feature level: {}\n", str[i]));
			break;
		}
	}
	assert(mDevice);

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(mDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// エラーや警告が出たら停止
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		// どうしようもないエラーは無視する
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_MESSAGE_ID ids[] = { D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		filter.DenyList.NumIDs = _countof(ids);
		filter.DenyList.pIDList = ids;
		infoQueue->PushStorageFilter(&filter);
	}
#endif

	// コマンドキューの作成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	hr = mDevice->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&mCmdQueue));
	assert(SUCCEEDED(hr));
	// コマンドアロケータの作成
	hr = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCmdAllocator));
	assert(SUCCEEDED(hr));
	// コマンドリストの作成
	hr = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&mCmdList));
	assert(SUCCEEDED(hr));

	Window& window = Window::GetInstance();
	uint32_t windowWidth = window.GetClientWidth();
	uint32_t windowHeight = window.GetClientHeight();

	// スワップチェーンの作成
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = windowWidth;
	swapChainDesc.Height = windowHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	hr = mFactory->CreateSwapChainForHwnd(mCmdQueue.Get(), window.GetHWnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(mSwapChain.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// RTV用デスクリプタヒープの作成
	mRTVHeap = std::make_unique<DescriptorHeap>();
	mRTVHeap->Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 128, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	// DSV用デスクリプタヒープの作成
	mDSVHeap = std::make_unique<DescriptorHeap>();
	mDSVHeap->Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 64, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	// SRV用デスクリプタヒープの作成
	mSRVHeap = std::make_unique<DescriptorHeap>();
	mSRVHeap->Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	for (uint32_t i = 0; i < 2; ++i) {
		// スワップチェーンからバックバッファを取得
		hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mBackBuffs[i]));
		assert(SUCCEEDED(hr));
		// レンダーターゲットビューの作成
		mRTVs[i] = mRTVHeap->Allocate();
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		mDevice->CreateRenderTargetView(mBackBuffs[i].Get(), &rtvDesc, mRTVs[i]->mCPU);
	}

	// フェンスの作成
	hr = mDevice->CreateFence(mFenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
	assert(SUCCEEDED(hr));
	// フェンスイベントの作成
	mFenceEvent = CreateEvent(nullptr, false, false, nullptr);
	assert(mFenceEvent);

	// ビューポートの設定
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<FLOAT>(windowWidth);
	mViewport.Height = static_cast<FLOAT>(windowHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	// シザー矩形の設定
	mScissorRc.left = 0;
	mScissorRc.top = 0;
	mScissorRc.right = windowWidth;
	mScissorRc.bottom = windowHeight;
}

void DirectXBase::Terminate() {
	if (mFenceEvent) {
		CloseHandle(mFenceEvent);
		mFenceEvent = nullptr;
	}
	for (auto dh : mRTVs) {
		mRTVHeap->Free(dh);
	}
	DirectXCommon::Terminate();
}

// フレームの開始
void DirectXBase::BeginFrame() {
	mSRVHeap->Set(mCmdList);
}

// 最終レンダーターゲットをセット
void DirectXBase::SetFinalRT() {
	// 現在のバックバッファのインデックスを取得
	mBackBuffIdx = mSwapChain->GetCurrentBackBufferIndex();
	// バックバッファを表示からレンダーターゲットへ
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mBackBuffs[mBackBuffIdx].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	mCmdList->ResourceBarrier(1, &barrier);
	// レンダーターゲットをセット
	mCmdList->OMSetRenderTargets(1, &mRTVs[mBackBuffIdx]->mCPU, false, nullptr);
	// レンダーターゲットをクリア
	float clearColor[4] = { 0.0f,0.0f,1.0f,0.0f };
	mCmdList->ClearRenderTargetView(mRTVs[mBackBuffIdx]->mCPU, clearColor, 0, nullptr);
	// ビューポートとシザー矩形をセット
	mCmdList->RSSetViewports(1, &mViewport);
	mCmdList->RSSetScissorRects(1, &mScissorRc);
}

// コマンドリストを実行
void DirectXBase::ExecuteCmdList(bool isFlip, UINT syncInterval) {
	if (isFlip) {
		// バックバッファをレンダーターゲットから表示へ
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Transition.pResource = mBackBuffs[mBackBuffIdx].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		mCmdList->ResourceBarrier(1, &barrier);
	}
	// コマンドリストを実行
	mCmdList->Close();
	ID3D12CommandList* cmdLists[] = { mCmdList.Get() };
	mCmdQueue->ExecuteCommandLists(1, cmdLists);
	if (isFlip) {
		mSwapChain->Present(syncInterval, 0);
	}
	// GPUの実行完了を待つ
	++mFenceVal;
	mCmdQueue->Signal(mFence.Get(), mFenceVal);
	if (mFence->GetCompletedValue() < mFenceVal) {
		mFence->SetEventOnCompletion(mFenceVal, mFenceEvent);
		WaitForSingleObject(mFenceEvent, INFINITE);
	}
	// コマンドリストをリセット
	mCmdAllocator->Reset();
	mCmdList->Reset(mCmdAllocator.Get(), nullptr);
}

DirectXBase::~DirectXBase() {
	// 解放できていないデスクリプタハンドルの数を出力
	Helper::Log(std::format("RTV: {}\n", mRTVHeap->GetActiveNum()));
	Helper::Log(std::format("DSV: {}\n", mDSVHeap->GetActiveNum()));
	Helper::Log(std::format("SRV: {}\n", mSRVHeap->GetActiveNum()));
}
