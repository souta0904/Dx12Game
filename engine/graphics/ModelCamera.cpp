#include "ModelCamera.h"

void ModelCamera::Create() {
	mCB = std::make_unique<ConstantBuffer>();
	mCB->Create(sizeof(Vector3));
}

void ModelCamera::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx) {
	mCB->Update(mTranslate);
	mCB->Set(cmdList, idx);
}
