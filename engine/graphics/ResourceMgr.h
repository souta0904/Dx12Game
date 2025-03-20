#pragma once
#include "Texture.h"
#include "Model.h"
#include <d3d12.h>
#include <dxcapi.h>
#include <memory>
#include <unordered_map>
#include <wrl.h>

// シェーダ
class Shader {
public:
	// コンパイル
	bool Compile(
		const std::string& path,
		const std::string& profile,
		Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils,
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler,
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler
	);
	Microsoft::WRL::ComPtr<IDxcBlob> GetBlob() const { return mBlob; }
private:
	// シェーダオブジェクト
	Microsoft::WRL::ComPtr<IDxcBlob> mBlob;
};

// リソース管理
class ResourceMgr final {
public:
	// シングルトンを実装
	static ResourceMgr& GetInstance() {
		static ResourceMgr instance;
		return instance;
	}
	ResourceMgr(const ResourceMgr&) = delete;
	ResourceMgr& operator=(const ResourceMgr&) = delete;
	ResourceMgr(ResourceMgr&&) = delete;
	ResourceMgr& operator=(ResourceMgr&&) = delete;
private:
	ResourceMgr() = default;
	~ResourceMgr() = default;

public:
	void Initialize();
	void Terminate();

	// リソースの取得
	Model* GetModel(const std::string& path);
	Shader* GetShader(const std::string& path, const std::string& profile);
	Texture* GetTexture(const std::string& path);

private:
	// リソース
	std::unordered_map<std::string, std::unique_ptr<Model>> mModels;// モデル
	std::unordered_map<std::string, std::unique_ptr<Shader>> mShaders;// シェーダ
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;// テクスチャ
	// シェーダコンパイラ
	Microsoft::WRL::ComPtr<IDxcUtils> mDxcUtils;
	Microsoft::WRL::ComPtr<IDxcCompiler3> mDxcCompiler;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> mIncludeHandler;
};
