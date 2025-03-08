#pragma once
#include <d3d12.h>

namespace DirectXCommon {

	// ヒープ設定
	extern D3D12_HEAP_PROPERTIES gHeapDefault;
	extern D3D12_HEAP_PROPERTIES gHeapUpload;

	// ブレンド
	extern D3D12_BLEND_DESC gBlendNone;
	extern D3D12_BLEND_DESC gBlendNormal;
	extern D3D12_BLEND_DESC gBlendAddition;
	extern D3D12_BLEND_DESC gBlendSubtract;
	extern D3D12_BLEND_DESC gBlendMultiply;
	extern D3D12_BLEND_DESC gBlendScreen;

	// ラスタライザ
	extern D3D12_RASTERIZER_DESC gRasterizerDefault;
	extern D3D12_RASTERIZER_DESC gRasterizerCullModeNone;
	extern D3D12_RASTERIZER_DESC gRasterizerFillModeWireframe;

	// 深度ステンシル
	extern D3D12_DEPTH_STENCIL_DESC gDepthEnable;
	extern D3D12_DEPTH_STENCIL_DESC gDepthDisable;
	extern D3D12_DEPTH_STENCIL_DESC gDepthWriteMaskZero;

	// サンプラー
	extern D3D12_STATIC_SAMPLER_DESC gSamplerLinearWrap;
	extern D3D12_STATIC_SAMPLER_DESC gSamplerLinearClamp;

}
