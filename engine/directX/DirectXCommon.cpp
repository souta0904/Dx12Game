#include "DirectXCommon.h"

namespace DirectXCommon {

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES gHeapDefault = {};
	D3D12_HEAP_PROPERTIES gHeapUpload = {};

	// ブレンド
	D3D12_BLEND_DESC gBlendNone = {};
	D3D12_BLEND_DESC gBlendNormal = {};
	D3D12_BLEND_DESC gBlendAddition = {};
	D3D12_BLEND_DESC gBlendSubtract = {};
	D3D12_BLEND_DESC gBlendMultiply = {};
	D3D12_BLEND_DESC gBlendScreen = {};

	// ラスタライザ
	D3D12_RASTERIZER_DESC gRasterizerDefault = {};
	D3D12_RASTERIZER_DESC gRasterizerCullModeNone = {};
	D3D12_RASTERIZER_DESC gRasterizerFillModeWireframe = {};

	// 深度ステンシル
	D3D12_DEPTH_STENCIL_DESC gDepthEnable = {};
	D3D12_DEPTH_STENCIL_DESC gDepthDisable = {};
	D3D12_DEPTH_STENCIL_DESC gDepthWriteMaskZero = {};

	// サンプラー
	D3D12_STATIC_SAMPLER_DESC gSamplerLinearWrap = {};
	D3D12_STATIC_SAMPLER_DESC gSamplerLinearClamp = {};

	void Initialize() {

		// ヒーププロパティ
		gHeapDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
		gHeapUpload.Type = D3D12_HEAP_TYPE_UPLOAD;// アップロード可

		// ブレンド
		// ブレンドなし
		gBlendNone.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		// αブレンド
		gBlendNormal.AlphaToCoverageEnable = false;
		gBlendNormal.IndependentBlendEnable = false;
		gBlendNormal.RenderTarget[0].BlendEnable = true;
		gBlendNormal.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		gBlendNormal.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		gBlendNormal.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		gBlendNormal.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		gBlendNormal.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		gBlendNormal.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		gBlendNormal.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		// 加算
		gBlendAddition = gBlendNormal;
		gBlendAddition.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		gBlendAddition.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		gBlendAddition.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		// 減算
		gBlendSubtract = gBlendNormal;
		gBlendSubtract.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		gBlendSubtract.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		gBlendSubtract.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		// 乗算
		gBlendMultiply = gBlendNormal;
		gBlendMultiply.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		gBlendMultiply.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		gBlendMultiply.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		// スクリーン
		gBlendScreen = gBlendNormal;
		gBlendScreen.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		gBlendScreen.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		gBlendScreen.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

		// ラスタライザ
		gRasterizerDefault.FillMode = D3D12_FILL_MODE_SOLID;
		gRasterizerDefault.CullMode = D3D12_CULL_MODE_BACK;
		gRasterizerDefault.FrontCounterClockwise = false;
		gRasterizerDefault.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		gRasterizerDefault.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		gRasterizerDefault.DepthClipEnable = true;
		gRasterizerDefault.MultisampleEnable = false;
		gRasterizerDefault.AntialiasedLineEnable = false;
		gRasterizerDefault.ForcedSampleCount = 0;
		gRasterizerDefault.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		// カリングしない
		gRasterizerCullModeNone = gRasterizerDefault;
		gRasterizerCullModeNone.CullMode = D3D12_CULL_MODE_NONE;
		// ワイヤーフレーム
		gRasterizerFillModeWireframe = gRasterizerDefault;
		gRasterizerFillModeWireframe.FillMode = D3D12_FILL_MODE_WIREFRAME;

		// 深度ステンシル
		// 深度あり
		gDepthEnable.DepthEnable = true;
		gDepthEnable.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		gDepthEnable.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		gDepthEnable.StencilEnable = false;
		// 深度なし
		gDepthDisable.DepthEnable = false;
		// 書き込まない
		gDepthWriteMaskZero = gDepthEnable;
		gDepthWriteMaskZero.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		// サンプラー
		// 繰り返す
		gSamplerLinearWrap.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		gSamplerLinearWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		gSamplerLinearWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		gSamplerLinearWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		gSamplerLinearWrap.MipLODBias = 0.0f;
		gSamplerLinearWrap.MaxAnisotropy = 0;
		gSamplerLinearWrap.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		gSamplerLinearWrap.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		gSamplerLinearWrap.MinLOD = 0.0f;
		gSamplerLinearWrap.MaxLOD = D3D12_FLOAT32_MAX;
		gSamplerLinearWrap.ShaderRegister = 0;
		gSamplerLinearWrap.RegisterSpace = 0;
		gSamplerLinearWrap.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		// 0.0から1.0の範囲に収める
		gSamplerLinearClamp.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		gSamplerLinearClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		gSamplerLinearClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		gSamplerLinearClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		gSamplerLinearClamp.MipLODBias = 0.0f;
		gSamplerLinearClamp.MaxAnisotropy = 0;
		gSamplerLinearClamp.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		gSamplerLinearClamp.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		gSamplerLinearClamp.MinLOD = 0.0f;
		gSamplerLinearClamp.MaxLOD = D3D12_FLOAT32_MAX;
		gSamplerLinearClamp.ShaderRegister = 0;
		gSamplerLinearClamp.RegisterSpace = 0;
		gSamplerLinearClamp.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	}

	void Terminate() {

	}

}
