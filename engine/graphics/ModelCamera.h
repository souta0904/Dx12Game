#pragma once
#include "directX/Buffer.h"
#include "Camera.h"
#include "Vector3.h"
#include <d3d12.h>
#include <memory>
#include <wrl.h>

class ModelCamera : public Camera {
public:
	void Create();
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx);
private:
	std::unique_ptr<ConstantBuffer> mCB;
};
