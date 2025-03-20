#include "RenderTexture.h"
#include "directX/DirectXBase.h"
#include "directX/DirectXCommon.h"
#include <cassert>

void RenderTexture::Create(uint32_t width, uint32_t height, bool useDepth) {
	mWidth = width;
	mHeight = height;
	mUseDepth = useDepth;

	mClearColor = Vector4(0.0f, 0.0f, 0.2f, 1.0f);// 暗めの青
	// レンダーターゲットの作成
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	D3D12_CLEAR_VALUE clearVal = {};
	clearVal.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearVal.Color[0] = mClearColor.x;
	clearVal.Color[1] = mClearColor.y;
	clearVal.Color[2] = mClearColor.z;
	clearVal.Color[3] = mClearColor.w;
	DirectXBase& directXBase = DirectXBase::GetInstance();
	Microsoft::WRL::ComPtr<ID3D12Device> device = DirectXBase::GetInstance().GetDevice();
	// シェーダリソースとして作成
	[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(&DirectXCommon::gHeapDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clearVal, IID_PPV_ARGS(&mBuff));
	assert(SUCCEEDED(hr));

	// RTVの作成
	mRTV = directXBase.GetRTVHeap()->Allocate();
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	device->CreateRenderTargetView(mBuff.Get(), &rtvDesc, mRTV->mCPU);
	// SRVの作成
	mSRV = directXBase.GetSRVHeap()->Allocate();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(mBuff.Get(), &srvDesc, mSRV->mCPU);

	// ビューポート
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<FLOAT>(mWidth);
	mViewport.Height = static_cast<FLOAT>(mHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	// シザー矩形
	mScissorRc.left = 0;
	mScissorRc.top = 0;
	mScissorRc.right = mWidth;
	mScissorRc.bottom = mHeight;

	if (mUseDepth) {
		// 深度バッファの作成
		desc.Format = DXGI_FORMAT_D32_FLOAT;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		clearVal.Format = DXGI_FORMAT_D32_FLOAT;
		clearVal.DepthStencil.Depth = 1.0f;
		hr = device->CreateCommittedResource(&DirectXCommon::gHeapDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearVal, IID_PPV_ARGS(&mDepthBuff));
		assert(SUCCEEDED(hr));

		// DSVの作成
		mDSV = directXBase.GetDSVHeap()->Allocate();
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		device->CreateDepthStencilView(mDepthBuff.Get(), &dsvDesc, mDSV->mCPU);
	}
}

void RenderTexture::Begin(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	// シェーダリソースからレンダーターゲットへ
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mBuff.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	cmdList->ResourceBarrier(1, &barrier);
	// レンダーターゲットをセット
	if (mUseDepth) {// 深度あり
		cmdList->OMSetRenderTargets(1, &mRTV->mCPU, false, &mDSV->mCPU);
		cmdList->ClearDepthStencilView(mDSV->mCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	} else {// 深度なし
		cmdList->OMSetRenderTargets(1, &mRTV->mCPU, false, nullptr);
	}
	cmdList->ClearRenderTargetView(mRTV->mCPU, &mClearColor.x, 0, nullptr);
	// ビューポートとシザー矩形
	cmdList->RSSetViewports(1, &mViewport);
	cmdList->RSSetScissorRects(1, &mScissorRc);
}

void RenderTexture::End(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = mBuff.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	cmdList->ResourceBarrier(1, &barrier);
}

void RenderTexture::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx) {
	cmdList->SetGraphicsRootDescriptorTable(idx, mSRV->mGPU);
}
