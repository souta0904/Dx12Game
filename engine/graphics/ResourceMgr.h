#pragma once
#include "Texture.h"
#include "Model.h"
#include <d3d12.h>
#include <dxcapi.h>
#include <memory>
#include <unordered_map>

class ResourceMgr {
public:
	static ResourceMgr& GetInstance() {
		static ResourceMgr instance;
		return instance;
	}
	ResourceMgr(const ResourceMgr&) = delete;
	ResourceMgr& operator=(const ResourceMgr&) = delete;
	ResourceMgr(ResourceMgr&&) = delete;
	ResourceMgr& operator=(ResourceMgr&&) = delete;

	void Initialize();
	void Terminate();

	Texture* GetTexture(const std::string& path);
	ModelData* GetModelData(const std::string& path);
	Microsoft::WRL::ComPtr<IDxcBlob> GetShader(const std::string& path, const std::string& profile);

private:
	ResourceMgr() = default;
	~ResourceMgr() = default;

private:
	Microsoft::WRL::ComPtr<IDxcUtils> mDxcUtils;
	Microsoft::WRL::ComPtr<IDxcCompiler3> mDxcCompiler;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> mIncludeHandler;

	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, std::unique_ptr<ModelData>> mModels;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> mShaders;
};
