#pragma once
#include "directX/Buffer.h"
#include "Matrix4.h"
#include "PSOFlags.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <array>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <d3d12.h>
#include <memory>
#include <string>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>
#include <wrl.h>

class Texture;
class ModelCamera;

enum class SpecularType {
	kNone,
	kPhong,
	kBlinnPhong
};

class Material {
public:
	friend class ModelData;
	friend class Model;

	struct Constant {
		Vector4 mColor;
		Matrix4 mUVMat;
		uint32_t mIsLighting;
		SpecularType mSpecularType;
		float mShininess;
	};
	void Create();
	void Update();
	void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t rootParamIdx);

public:
	std::string mName;
	Vector4 mColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Texture* mTexture;
	Vector2 mUVScale = Vector2(1.0f, 1.0f);
	float mUVRotate = 0.0f;
	Vector2 mUVTranslate = Vector2(0.0f, 0.0f);
	bool mIsLighting = true;
	SpecularType mSpecularType = SpecularType::kPhong;
	float mShininess = 100.0f;

private:
	std::unique_ptr<ConstantBuffer> mConstantBuffer;
};

class Mesh {
public:
	friend class ModelData;
	friend class Model;

	struct Vertex {
		Vector4 mPosition;
		Vector3 mNormal;
		Vector2 mUV;
	};
	struct SkinVertex {
		Vector4 mPosition;
		Vector3 mNormal;
		Vector2 mUV;
		std::array<float, 4> mWeights;
		std::array<int32_t, 4> mBoneIndices;
	};
	void Create();
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);

private:
	std::string mName;
	bool mHasSkin;
	std::vector<Vertex> mVertices;
	std::vector<SkinVertex> mSkinVertices;
	std::unique_ptr<VertexBuffer> mVertexBuffer;
	std::vector<uint32_t> mIndices;
	std::unique_ptr<IndexBuffer> mIndexBuffer;
	uint32_t mMaterialIndex;
};

template <typename T>
struct Keyframe {
	float mTime;
	T mValue;
};
template <typename T>
struct AnimationCurve {
	std::vector<Keyframe<T>> mKeyframes;
};
struct NodeAnimation {
	AnimationCurve<Vector3> mScale;
	AnimationCurve<Quaternion> mRotate;
	AnimationCurve<Vector3> mTranslate;
};
struct Animation {
	std::string mName;
	std::unordered_map<std::string, NodeAnimation> mNodeAnimations;
	float mDuration;
};

class ModelData {
public:
	friend class Model;
	struct Node {
		aiNode* mAssimpNode;
		std::string mName;
		Vector3 mScale;
		Quaternion mRotate;
		Vector3 mTranslate;
		Matrix4 mLocalMat;
		Matrix4 mWorldMat;
		int32_t mIndex;
		std::optional<int32_t> mParent;
		std::vector<int32_t> mChildren;
	};
	struct MeshNode {
		int32_t mNodeIndex;
		std::unique_ptr<Mesh> mMesh;
	};
	struct BoneNode {
		int32_t mNodeIndex;
		Matrix4 mInverseBindMat;
	};

	bool Create(const std::string& path);

private:
	int32_t CreateNode(aiNode* srcNode, const std::optional<int32_t>& parent);
	void Output(Node* node, uint32_t depth = 0);

	void CreateMesh(Node* node);
	void CreateMaterial();
	void CreateAnimation();

private:
	const aiScene* mAssimpScene;
	std::unordered_map<std::string, int32_t> mNodeMap;

	std::string mPath;
	std::vector<Node> mSrcNodes;
	std::vector<std::unique_ptr<Material>> mSrcMaterials;
	std::unordered_map<std::string, std::unique_ptr<Animation>> mSrcAnimations;
	int32_t mRootIndex;
	std::vector<MeshNode> mMeshes;
	std::vector<BoneNode> mBones;
};

class Model {
public:
	struct TransformationMatrix {
		Matrix4 mWorldMat;
		Matrix4 mWVPMat;
		Matrix4 mWorldInverseTransposeMat;
	};

	void Create(ModelData* srcModel);
	void Update(float deltaTime);
	void Draw(const Matrix4& worldMat, ModelCamera* camera = nullptr);
	void Play(const std::string& animationName, float rate = 1.0f);
	void SetMaterial(uint32_t idx, Material* material);

	PSOFlags GetPSOFlags() const { return mFlags; }
	void SetPSOFlags(PSOFlags flags) { mFlags = flags; }

private:
	void UpdateNode(ModelData::Node* node);
	void UpdatePalette();
	Vector3 UpdateAnimation(const AnimationCurve<Vector3>& animation, float time);
	Quaternion UpdateAnimation(const AnimationCurve<Quaternion>& animation, float time);

private:
	ModelData* mSrcModel;
	std::vector<ModelData::Node> mNodes;
	std::vector<Material*> mMaterials;
	std::vector<std::unique_ptr<ConstantBuffer>> mTransformBuff;
	std::unique_ptr<Animation> mAnimation;
	float mAnimationTime;
	float mPlayRate;
	bool mIsLoop = true;
	std::unique_ptr<StructuredBuffer> mPaletteBuff;

	PSOFlags mFlags;
};
