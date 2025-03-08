#pragma once
#include "directX/Buffer.h"
#include "directX/PipelineState.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cstdint>

// ライン描画
class LineRenderer {
public:
	static LineRenderer& GetInstance() {
		static LineRenderer instance;
		return instance;
	}
	LineRenderer(const LineRenderer&) = delete;
	LineRenderer& operator=(const LineRenderer&) = delete;
	LineRenderer(LineRenderer&&) = delete;
	LineRenderer& operator=(LineRenderer&&) = delete;

	struct Vertex {
		Vector4 mPosition;
		Vector4 mColor;
	};

	void Initialize();
	void Terminate();
	void Prepare();
	void Render();

	void DrawLine3(const Vector3& p1, const Vector3& p2, const Vector4& color = Vector4::kOne);
	void DrawGrid();// グリッド線

private:
	LineRenderer() = default;
	~LineRenderer() = default;

private:
	std::unique_ptr<RootSignature> mRS;
	std::unique_ptr<PipelineState> mPS;
	std::unique_ptr<ConstantBuffer> mCB;
	static const uint32_t kVertNum = 1000000;// 最大長点数
	std::unique_ptr<VertexBuffer> mVB;
	std::vector<Vertex> mVertices;
	uint32_t mVertIdx;
};
