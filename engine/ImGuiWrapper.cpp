#include "ImGuiWrapper.h"
#include "directX/DirectXBase.h"
#include "Window.h"

void ImGuiWrapper::Initialize() {
	Window& window = Window::GetInstance();
	if (!ImGui::CreateContext()) {
		assert(false);
	}
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if (!ImGui_ImplWin32_Init(window.GetHWnd())) {
		assert(false);
	}
	DirectXBase& directXBase = DirectXBase::GetInstance();
	DescriptorHeap* srvHeap = directXBase.GetSRVHeap();
	mSRV = srvHeap->Allocate();
	if (!ImGui_ImplDX12_Init(directXBase.GetDevice().Get(), 2, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvHeap->GetDescriptorHeap().Get(), mSRV->mCPU, mSRV->mGPU)) {
		assert(false);
	}
}

void ImGuiWrapper::Terminate() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiWrapper::Begin() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiWrapper::End() {
	ImGui::Render();
}

void ImGuiWrapper::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());
}
