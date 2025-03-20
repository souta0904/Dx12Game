#pragma once
#include "directX/DescriptorHeap.h"
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx12.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_stdlib.h>
#include <d3d12.h>
#include <wrl.h>

class ImGuiWrapper {
public:
	void Initialize();
	void Terminate();
	void Begin();
	void End();
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
private:
	DescriptorHandle* mSRV;
};
