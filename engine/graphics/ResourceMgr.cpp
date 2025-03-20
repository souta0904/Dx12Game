#include "ResourceMgr.h"
#include "Helper.h"
#include <cassert>
#include <format>

#pragma comment(lib, "dxcompiler.lib")

void ResourceMgr::Initialize() {
	// コンパイラを初期化
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&mDxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mDxcCompiler));
	assert(SUCCEEDED(hr));
	hr = mDxcUtils->CreateDefaultIncludeHandler(&mIncludeHandler);
	assert(SUCCEEDED(hr));
}

void ResourceMgr::Terminate() {

}

// モデル
Model* ResourceMgr::GetModel(const std::string& path) {
	auto it = mModels.find(path);
	if (it != mModels.end()) {
		return it->second.get();
	} else {
		std::unique_ptr<Model> model = std::make_unique<Model>();
		if (model->Create(path)) {
			Model* ptr = model.get();
			mModels.emplace(path, std::move(model));
			return ptr;
		}
	}
	return nullptr;
}

// シェーダ
Shader* ResourceMgr::GetShader(const std::string& path, const std::string& profile) {
	auto it = mShaders.find(path);
	if (it != mShaders.end()) {
		return it->second.get();
	} else {
		// シェーダの作成、コンパイル
		std::unique_ptr<Shader> shader = std::make_unique<Shader>();
		if (shader->Compile(path, profile, mDxcUtils, mDxcCompiler, mIncludeHandler)) {
			Shader* ptr = shader.get();
			mShaders.emplace(path, std::move(shader));
			return ptr;
		}
	}
	return nullptr;
}

// テクスチャ
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

// シェーダのコンパイル
bool Shader::Compile(
	const std::string& path,
	const std::string& profile,
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils,
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler,
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler
) {
	// hlslファイルを読み込む
	std::wstring wpath = Helper::ConvertString(path);
	std::wstring wprofile = Helper::ConvertString(profile);
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> source = nullptr;
	HRESULT hr = dxcUtils->LoadFile(wpath.c_str(), nullptr, &source);
	if (FAILED(hr)) {
		return false;
	}

	DxcBuffer sourceBuff = {};
	sourceBuff.Ptr = source->GetBufferPointer();
	sourceBuff.Size = source->GetBufferSize();
	sourceBuff.Encoding = DXC_CP_UTF8;
	LPCWSTR options[] = {// コンパイルオプション
		wpath.c_str(),
		L"-E", L"main",
		L"-T", wprofile.c_str(),
		L"-Zi", L"-Qembed_debug",
		L"-Od",
		L"-Zpr"
	};
	// シェーダをコンパイル
	Microsoft::WRL::ComPtr<IDxcResult> result = nullptr;
	hr = dxcCompiler->Compile(&sourceBuff, options, _countof(options), includeHandler.Get(), IID_PPV_ARGS(&result));
	if (FAILED(hr)) {
		return false;
	}

	// エラーや警告の出力
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> error = nullptr;
	result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&error), nullptr);
	if (error && error->GetStringLength() > 0) {
		Helper::Log(error->GetStringPointer());
		return false;
	}

	// シェーダオブジェクトを取得
	hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&mBlob), nullptr);
	if (FAILED(hr)) {
		return false;
	}

	Helper::Log(std::format("Compile '{}'\n", path));
	return true;
}
