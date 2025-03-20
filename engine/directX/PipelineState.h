#pragma once
#include <cstdint>
#include <d3d12.h>
#include <dxcapi.h>
#include <memory>
#include <vector>
#include <wrl.h>

class Shader;

// ルートパラメータ
class RootParameter {
public:
	friend class RootSignature;
	~RootParameter();
	void InitCBV(uint32_t registerNum, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitDescriptorTable(uint32_t rangeNum, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitDescriptorRange(uint32_t idx, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t descNum, uint32_t baseRegisterNum);
private:
	D3D12_ROOT_PARAMETER mRootParameter;
};

// ルートシグネチャ
class RootSignature {
public:
	RootSignature(uint32_t parameterNum, uint32_t samplerNum);
	// ルートパラメータにアクセス
	RootParameter& GetParameter(uint32_t idx);
	// サンプラーにアクセス
	D3D12_STATIC_SAMPLER_DESC& GetSampler(uint32_t idx);
	void Create();
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const { return mRootSignature; }

private:
	uint32_t mParameterNum;
	std::unique_ptr<RootParameter[]> mParameters;
	uint32_t mSamplerNum;
	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> mSamplers;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
};

// パイプライン初期化用
struct PSOInit {
	RootSignature* mRootSignature;
	Shader* mVertexShader;
	Shader* mPixelShader;
	D3D12_BLEND_DESC mBlendDesc;
	D3D12_RASTERIZER_DESC mRasterizerDesc;
	D3D12_DEPTH_STENCIL_DESC mDepthStencilDesc;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayouts;
	// デフォルトでD3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
	D3D12_PRIMITIVE_TOPOLOGY_TYPE mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
};

// パイプラインステート
class PipelineState {
public:
	PipelineState();
	// コピー、代入の定義
	PipelineState(const PipelineState& pipelineState);
	PipelineState& operator=(const PipelineState& pipelineState);

	void Create(const PSOInit& init);
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC mDesc;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayouts;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
};
