#pragma once
#include "directX/Buffer.h"
#include "Camera.h"
#include <memory>

// モデル用カメラ
class ModelCamera : public Camera {
public:
	void Create();
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx);
private:
	// カメラ座標用定数バッファ
	std::unique_ptr<ConstantBuffer> mCB;
};
