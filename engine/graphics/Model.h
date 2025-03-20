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
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <wrl.h>

class Texture;
class ModelCamera;

// 鏡面反射の種類
enum class SpecularType {
	kNone,
	kPhong,		// フォン反射
	kBlinnPhong	// ブリン・フォン反射
};

// マテリアル
class Material {
public:
	friend class Model;
	friend class ModelInstance;
	// シェーダ用定数
	struct Constant {
		Vector4 mColor;
		Matrix4 mUVMat;
		uint32_t mIsLighting;
		SpecularType mSpecularType;
		float mShininess;
	};
	void Create();
	void Update();
	void Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx);
public:
	// マテリアル名
	std::string mName;
	// 色
	Vector4 mColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// テクスチャ
	Texture* mTexture;
	// UVトランスフォーム
	Vector2 mUVScale = Vector2(1.0f, 1.0f);
	float mUVRotate = 0.0f;
	Vector2 mUVTranslate = Vector2(0.0f, 0.0f);
	// ライティングするか
	bool mIsLighting = true;
	// 鏡面反射の種類
	SpecularType mSpecularType = SpecularType::kPhong;
	// 光沢度
	float mShininess = 100.0f;
private:
	// 定数バッファ
	std::unique_ptr<ConstantBuffer> mCB;
};

// メッシュ
class Mesh {
public:
	friend class Model;
	friend class ModelInstance;
	// 頂点
	struct Vertex {
		Vector4 mPosition;
		Vector3 mNormal;
		Vector2 mUV;
	};
	struct SkinVertex {// スキン用
		Vector4 mPosition;
		Vector3 mNormal;
		Vector2 mUV;
		std::array<float, 4> mWeights;
		std::array<int32_t, 4> mBoneIndices;
	};
	void Create();
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
private:
	// メッシュ名
	std::string mName;
	// スキンの有無
	bool mHasSkin;
	// 頂点
	std::vector<Vertex> mVertices;
	std::vector<SkinVertex> mSkinVertices;// スキン用
	std::unique_ptr<VertexBuffer> mVB;
	// インデックス
	std::vector<uint32_t> mIndices;
	std::unique_ptr<IndexBuffer> mIB;
	// マテリアルリストのインデックス
	uint32_t mMaterialIdx;
};

// アニメーション関連
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
	std::string mName;// アニメーション名
	std::unordered_map<std::string, NodeAnimation> mNodeAnimations;
	float mDuration;// 尺
};

// モデル(元データ)
class Model {
public:
	friend class ModelInstance;
	// モデルノード
	struct Node {
		aiNode* mAssimpNode;// 読み込み用

		std::string mName;// ノード名
		// トランスフォーム
		Vector3 mScale;
		Quaternion mRotate;
		Vector3 mTranslate;
		Matrix4 mLocalMat;
		Matrix4 mWorldMat;
		// 親子関係
		int32_t mIdx;// 自分のインデックス
		std::optional<int32_t> mParent;// 親のインデックス
		std::vector<int32_t> mChildren;// 子のインデックス
	};
	// メッシュ用ノード
	struct MeshNode {
		int32_t mNodeIdx;
		std::unique_ptr<Mesh> mMesh;
	};
	// ボーン用ノード
	struct BoneNode {
		int32_t mNodeIdx;
		Matrix4 mInverseBindMat;// 逆バインドポーズ行列
	};
	bool Create(const std::string& path);
private:
	// 再帰でノードを作成
	int32_t CreateNode(aiNode* srcNode, const std::optional<int32_t>& parent);
	void Output(Node* node, uint32_t depth = 0);// デバッグ用
	// メッシュ、マテリアル、アニメーションの作成
	void CreateMesh(Node* node);
	void CreateMaterial();
	void CreateAnimation();
private:
	// 読み込み用
	const aiScene* mAssimpScene;
	// ノード名とインデックスのペア
	std::unordered_map<std::string, int32_t> mNodeMap;

	std::string mPath;
	std::vector<Node> mSrcNodes;// ノードリスト
	int32_t mRootIdx;
	std::vector<std::unique_ptr<Material>> mSrcMaterials;// マテリアル
	std::map<std::string, std::unique_ptr<Animation>> mSrcAnimations;// アニメーション
	std::vector<MeshNode> mMeshes;// メッシュ
	std::vector<BoneNode> mBones;// ボーン
};

// モデル(インスタンス化)
class ModelInstance {
public:
	// 変換行列
	struct TransformationMatrix {
		Matrix4 mWorldMat;// ワールド行列
		Matrix4 mWVPMat;// ワールドビュープロジェクション行列
		Matrix4 mWorldInverseTransposeMat;// ワールド行列の逆転置行列
	};

	void Create(Model* src);// インスタンス化
	void Update(float deltaTime);
	void Draw(const Matrix4& worldMat, ModelCamera* camera = nullptr);
	// アニメーション再生
	void PlayAtName(const std::string& name, float rate = 1.0f);
	void PlayAtIdx(uint32_t idx, float rate = 1.0f);
	// マテリアル割り当て
	void SetMaterial(uint32_t idx, Material* material);

	Model* GetModel() const { return mSrc; }
	PSOFlags GetFlags() const { return mFlags; }
	// モデルの再セットは不可(インスタンス化してください)
	//void SetModel(Model* src);// cpp
	void SetFlags(PSOFlags flags) { mFlags = flags; }

private:
	// アニメーション更新のヘルパー関数
	void UpdateNode(Model::Node* node);
	void UpdatePalette();
	Vector3 UpdateAnimation(const AnimationCurve<Vector3>& animCurve, float time);
	Quaternion UpdateAnimation(const AnimationCurve<Quaternion>& animCurve, float time);

private:
	// モデル元データ
	Model* mSrc;
	// ノード(元データからコピー)
	std::vector<Model::Node> mNodes;
	// 変換行列
	std::vector<std::unique_ptr<ConstantBuffer>> mTransformationMatBuff;
	// マテリアル
	std::vector<Material*> mMaterials;
	// アニメーション関連
	std::unique_ptr<Animation> mCurrAnim;
	float mAnimTime;
	std::unique_ptr<StructuredBuffer> mMatPaletteBuff;
public:
	// 再生速度
	float mPlayRate;
	// ループするか
	bool mIsLoop = true;
	// PSOフラグ
	PSOFlags mFlags;
};
