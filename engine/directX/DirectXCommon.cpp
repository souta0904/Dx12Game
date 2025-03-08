#include "DirectXCommon.h"

namespace DirectXCommon {

	// ヒープ設定
	D3D12_HEAP_PROPERTIES gHeapDefault = { D3D12_HEAP_TYPE_DEFAULT };
	D3D12_HEAP_PROPERTIES gHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };// アップロード

	// ブレンド
	// ブレンドなし
	D3D12_BLEND_DESC gBlendNone = {
		.RenderTarget = {
			{
				.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
			}
		}
	};
	// αブレンド
	D3D12_BLEND_DESC gBlendNormal = {
		.AlphaToCoverageEnable = false,
		.IndependentBlendEnable = false,
		.RenderTarget = {
			{
				.BlendEnable = true,
				.SrcBlend = D3D12_BLEND_SRC_ALPHA,
				.DestBlend = D3D12_BLEND_INV_SRC_ALPHA,
				.BlendOp = D3D12_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D12_BLEND_ONE,
				.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA,
				.BlendOpAlpha = D3D12_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
			}
		}
	};
	// 加算
	D3D12_BLEND_DESC gBlendAddition = {
		.AlphaToCoverageEnable = false,
		.IndependentBlendEnable = false,
		.RenderTarget = {
			{
				.BlendEnable = true,
				.SrcBlend = D3D12_BLEND_SRC_ALPHA,
				.DestBlend = D3D12_BLEND_ONE,
				.BlendOp = D3D12_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D12_BLEND_ONE,
				.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA,
				.BlendOpAlpha = D3D12_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
			}
		}
	};
	// 減算
	D3D12_BLEND_DESC gBlendSubtract = {
		.AlphaToCoverageEnable = false,
		.IndependentBlendEnable = false,
		.RenderTarget = {
			{
				.BlendEnable = true,
				.SrcBlend = D3D12_BLEND_SRC_ALPHA,
				.DestBlend = D3D12_BLEND_ONE,
				.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT,
				.SrcBlendAlpha = D3D12_BLEND_ONE,
				.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA,
				.BlendOpAlpha = D3D12_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
			}
		}
	};
	// 乗算
	D3D12_BLEND_DESC gBlendMultiply = {
		.AlphaToCoverageEnable = false,
		.IndependentBlendEnable = false,
		.RenderTarget = {
			{
				.BlendEnable = true,
				.SrcBlend = D3D12_BLEND_ZERO,
				.DestBlend = D3D12_BLEND_SRC_COLOR,
				.BlendOp = D3D12_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D12_BLEND_ONE,
				.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA,
				.BlendOpAlpha = D3D12_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
			}
		}
	};
	// スクリーン
	D3D12_BLEND_DESC gBlendScreen = {
		.AlphaToCoverageEnable = false,
		.IndependentBlendEnable = false,
		.RenderTarget = {
			{
				.BlendEnable = true,
				.SrcBlend = D3D12_BLEND_INV_DEST_COLOR,
				.DestBlend = D3D12_BLEND_ONE,
				.BlendOp = D3D12_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D12_BLEND_ONE,
				.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA,
				.BlendOpAlpha = D3D12_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
			}
		}
	};

	// ラスタライザ
	D3D12_RASTERIZER_DESC gRasterizerDefault = {
		.FillMode = D3D12_FILL_MODE_SOLID,
		.CullMode = D3D12_CULL_MODE_BACK,
		.FrontCounterClockwise = false,
		.DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
		.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		.DepthClipEnable = true,
		.MultisampleEnable = false,
		.AntialiasedLineEnable = false,
		.ForcedSampleCount = 0,
		.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};
	// カリングしない
	D3D12_RASTERIZER_DESC gRasterizerCullModeNone = {
		.FillMode = D3D12_FILL_MODE_SOLID,
		.CullMode = D3D12_CULL_MODE_NONE,
		.FrontCounterClockwise = false,
		.DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
		.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		.DepthClipEnable = true,
		.MultisampleEnable = false,
		.AntialiasedLineEnable = false,
		.ForcedSampleCount = 0,
		.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};
	// ワイヤーフレーム
	D3D12_RASTERIZER_DESC gRasterizerFillModeWireframe = {
		.FillMode = D3D12_FILL_MODE_WIREFRAME,
		.CullMode = D3D12_CULL_MODE_BACK,
		.FrontCounterClockwise = false,
		.DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
		.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		.DepthClipEnable = true,
		.MultisampleEnable = false,
		.AntialiasedLineEnable = false,
		.ForcedSampleCount = 0,
		.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

	// 深度ステンシル
	// 深度あり
	D3D12_DEPTH_STENCIL_DESC gDepthEnable = {
		.DepthEnable = true,
		.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
		.DepthFunc = D3D12_COMPARISON_FUNC_LESS,
		.StencilEnable = false
	};
	// 深度なし
	D3D12_DEPTH_STENCIL_DESC gDepthDisable = {
		.DepthEnable = false
	};
	// 書き込まない
	D3D12_DEPTH_STENCIL_DESC gDepthWriteMaskZero = {
		.DepthEnable = true,
		.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO,
		.DepthFunc = D3D12_COMPARISON_FUNC_LESS,
		.StencilEnable = false
	};

	// サンプラー
	// 繰り返す
	D3D12_STATIC_SAMPLER_DESC gSamplerLinearWrap = {
		.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		.MipLODBias = 0.0f,
		.MaxAnisotropy = 0,
		.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
		.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
		.MinLOD = 0.0f,
		.MaxLOD = D3D12_FLOAT32_MAX,
		.ShaderRegister = 0,
		.RegisterSpace = 0,
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
	};
	// 0.0から1.0の範囲に収める
	D3D12_STATIC_SAMPLER_DESC gSamplerLinearClamp = {
		.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		.MipLODBias = 0.0f,
		.MaxAnisotropy = 0,
		.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
		.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
		.MinLOD = 0.0f,
		.MaxLOD = D3D12_FLOAT32_MAX,
		.ShaderRegister = 0,
		.RegisterSpace = 0,
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
	};

}
