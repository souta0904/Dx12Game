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
	// シングルトンを実装
	static LineRenderer& GetInstance() {
		static LineRenderer instance;
		return instance;
	}
	LineRenderer(const LineRenderer&) = delete;
	LineRenderer& operator=(const LineRenderer&) = delete;
	LineRenderer(LineRenderer&&) = delete;
	LineRenderer& operator=(LineRenderer&&) = delete;
private:
	LineRenderer() = default;
	~LineRenderer() = default;

public:
	// 頂点
	struct Vertex {
		Vector4 mPosition;
		Vector4 mColor;
	};

	void Initialize();
	void Terminate();
	// ライン描画準備
	void Prepare();
	// まとめて描画
	void Render();

	void DrawLine3(const Vector3& start, const Vector3& end, const Vector4& color = Vector4::kOne);
	// グリッドを描画
	void DrawGrid(uint32_t dist = 1000);

private:
	std::unique_ptr<RootSignature> mRS;
	std::unique_ptr<PipelineState> mPS;
	std::unique_ptr<ConstantBuffer> mCB;// ビュープロジェクション行列
	std::unique_ptr<VertexBuffer> mVB;
	static const uint32_t kVertNum = 1048576;// 最大頂点数 2^20
	std::vector<Vertex> mVertices;
	uint32_t mVertIdx;
};
