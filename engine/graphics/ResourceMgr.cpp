#include "ResourceMgr.h"
#include "Helper.h"
#include <cassert>
#include <format>

#pragma comment(lib, "dxcompiler.lib")

void ResourceMgr::Initialize() {
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&mDxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mDxcCompiler));
	assert(SUCCEEDED(hr));
	hr = mDxcUtils->CreateDefaultIncludeHandler(&mIncludeHandler);
	assert(SUCCEEDED(hr));
}

void ResourceMgr::Terminate() {

}

Texture* ResourceMgr::GetTexture(const std::string& path) {
	auto it = mTextures.find(path);
	if (it != mTextures.end()) {
		return it->second.get();
	} else {
		std::unique_ptr<Texture> texture = std::make_unique<Texture>();
		if (texture->Create(path)) {
			Texture* ptr = texture.get();
			mTextures.emplace(path, std::move(texture));
			return ptr;
		}
	}
	return nullptr;
}

ModelData* ResourceMgr::GetModelData(const std::string& path) {
	auto it = mModels.find(path);
	if (it != mModels.end()) {
		return it->second.get();
	} else {
		std::unique_ptr<ModelData> model = std::make_unique<ModelData>();
		if (model->Create(path)) {
			ModelData* ptr = model.get();
			mModels.emplace(path, std::move(model));
			return ptr;
		}
	}
	return nullptr;
}

Microsoft::WRL::ComPtr<IDxcBlob> ResourceMgr::GetShader(const std::string& path, const std::string& profile) {
	auto it = mShaders.find(path);
	if (it != mShaders.end()) {
		return it->second;
	} else {
		std::wstring wpath = Helper::ConvertString(path);
		std::wstring wprofile = Helper::ConvertString(profile);
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> source = nullptr;
		HRESULT hr = mDxcUtils->LoadFile(wpath.c_str(), nullptr, &source);
		if (FAILED(hr)) {
			return nullptr;
		}

		DxcBuffer sourceBuff = {};
		sourceBuff.Ptr = source->GetBufferPointer();
		sourceBuff.Size = source->GetBufferSize();
		sourceBuff.Encoding = DXC_CP_UTF8;
		LPCWSTR options[] = {
			wpath.c_str(),
			L"-E", L"main",
			L"-T", wprofile.c_str(),
			L"-Zi", L"-Qembed_debug",
			L"-Od",
			L"-Zpr"
		};
		Microsoft::WRL::ComPtr<IDxcResult> compileResult = nullptr;
		hr = mDxcCompiler->Compile(&sourceBuff, options, _countof(options), mIncludeHandler.Get(), IID_PPV_ARGS(&compileResult));
		if (FAILED(hr)) {
			return nullptr;
		}

		Microsoft::WRL::ComPtr<IDxcBlobUtf8> errors = nullptr;
		compileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
		if (errors && errors->GetStringLength() > 0) {
			Helper::Log(errors->GetStringPointer());
			return nullptr;
		}

		Microsoft::WRL::ComPtr<IDxcBlob> shaderObj = nullptr;
		hr = compileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderObj), nullptr);
		if (FAILED(hr)) {
			return nullptr;
		}
		Helper::Log(std::format("Compile '{}'\n", path));

		if (shaderObj) {
			mShaders.emplace(path, shaderObj);
			return shaderObj;
		}
	}
	return nullptr;
}
