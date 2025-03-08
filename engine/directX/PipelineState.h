#pragma once
#include <cstdint>
#include <d3d12.h>
#include <dxcapi.h>
#include <memory>
#include <vector>
#include <wrl.h>

class RootParameter {
public:
	friend class RootSignature;
	~RootParameter();
	void InitCBV(uint32_t registerNum, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitDescriptorTable(uint32_t rangeNum, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitDescriptorRange(uint32_t idx, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t numDescs, uint32_t registerNum);
private:
	D3D12_ROOT_PARAMETER mRootParam;
};

class RootSignature {
public:
	RootSignature(uint32_t numParams, uint32_t numSamplers);
	RootParameter& Param(uint32_t idx);
	D3D12_STATIC_SAMPLER_DESC& Sampler(uint32_t idx);
	void Create();
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const { return mRootSignature; }
private:
	uint32_t mNumParams;
	std::unique_ptr<RootParameter[]> mParams;
	uint32_t mNumSamplers;
	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> mSamplers;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
};

class PipelineState {
public:
	PipelineState();
	PipelineState(const PipelineState& ps);
	PipelineState& operator=(const PipelineState& ps);
	void SetRootSignature(RootSignature* rs);
	void SetVertexShader(Microsoft::WRL::ComPtr<IDxcBlob> vs);
	void SetPixelShader(Microsoft::WRL::ComPtr<IDxcBlob> ps);
	void SetBlendState(D3D12_BLEND_DESC bs);
	void SetRasterizerState(D3D12_RASTERIZER_DESC rs);
	void SetDepthStencilState(D3D12_DEPTH_STENCIL_DESC dss);
	void SetInputLayout(uint32_t num, const D3D12_INPUT_ELEMENT_DESC* descs);
	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE ptt);
	void Create();
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC mDesc;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayouts;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;
};
