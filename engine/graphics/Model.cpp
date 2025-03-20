#include "Model.h"
#include "directX/DirectXBase.h"
#include "ModelBase.h"
#include "Helper.h"
#include "ModelCamera.h"
#include "ResourceMgr.h"
#include <cassert>
#include <format>

// マテリアル

void Material::Create() {
	mCB = std::make_unique<ConstantBuffer>();
	mCB->Create(sizeof(Constant));
	// 更新しておく
	Update();
}

// 定数バッファを更新
void Material::Update() {
	Constant data = {};
	data.mColor = mColor;
	data.mUVMat = CreateAffine4(
		Vector3(mUVScale.x, mUVScale.y, 1.0f),
		Quaternion(Vector3(0.0f, 0.0f, 1.0f), mUVRotate),
		Vector3(mUVTranslate.x, mUVTranslate.y, 0.0f)
	);
	data.mIsLighting = mIsLighting;
	data.mSpecularType = mSpecularType;
	data.mShininess = mShininess;
	mCB->Update(&data);
}

void Material::Set(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t idx) {
	mCB->Set(cmdList, idx);
}

// メッシュ

void Mesh::Create() {
	// 頂点バッファの作成
	if (mHasSkin) {// スキンあり
		mVB = std::make_unique<VertexBuffer>();
		mVB->Create(static_cast<uint32_t>(sizeof(SkinVertex) * mSkinVertices.size()), sizeof(SkinVertex), mSkinVertices.data());
	} else {// スキンなし
		mVB = std::make_unique<VertexBuffer>();
		mVB->Create(static_cast<uint32_t>(sizeof(Vertex) * mVertices.size()), sizeof(Vertex), mVertices.data());
	}
	// インデックスバッファの作成
	mIB = std::make_unique<IndexBuffer>();
	mIB->Create(sizeof(uint32_t) * static_cast<uint32_t>(mIndices.size()), mIndices.data());
}

void Mesh::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	mVB->Set(cmdList);// 頂点バッファ
	mIB->Set(cmdList);// インデックスバッファ
	cmdList->DrawIndexedInstanced(static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);// 描画
}

// モデル(元データ)

bool Model::Create(const std::string& path) {
	// assimpでモデルを読み込む
	mPath = path;
	Assimp::Importer importer = {};
	mAssimpScene = importer.ReadFile(path.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	if (!mAssimpScene->HasMeshes()) {
		return false;
	}

	// ノードを構築
	mRootIdx = CreateNode(mAssimpScene->mRootNode, {});
	bool isOutput = true;
	if (isOutput) {
		Helper::Log(std::format("'{}'\n", mPath));
		Output(&mSrcNodes[mRootIdx]);// デバッグ用
	}

	// メッシュの作成
	CreateMesh(&mSrcNodes[mRootIdx]);
	// マテリアルの作成
	CreateMaterial();

	importer.FreeScene();
	mNodeMap.clear();

	// アニメーションの作成
	CreateAnimation();

	Helper::Log(std::format("Create '{}'\n", mPath));
	return true;
}

// 再帰でノードを作成
int32_t Model::CreateNode(aiNode* srcNode, const std::optional<int32_t>& parent) {
	Node node = {};
	node.mAssimpNode = srcNode;// 読み込み用
	node.mName = srcNode->mName.C_Str();// ノード名
	// トランスフォーム
	aiVector3D scale, translate;
	aiQuaternion rotate;
	srcNode->mTransformation.Decompose(scale, rotate, translate);
	node.mScale = Vector3(scale.x, scale.y, scale.z);
	node.mRotate = Quaternion(rotate.x, -rotate.y, -rotate.z, rotate.w);
	node.mTranslate = Vector3(-translate.x, translate.y, translate.z);
	node.mLocalMat = CreateAffine4(node.mScale, node.mRotate, node.mTranslate);
	if (node.mParent) {
		node.mWorldMat = node.mLocalMat * mSrcNodes[*node.mParent].mWorldMat;
	} else {
		node.mWorldMat = node.mLocalMat;
	}
	// 親子関係
	node.mIdx = static_cast<uint32_t>(mSrcNodes.size());
	node.mParent = parent;
	mSrcNodes.emplace_back(node);// ノードを登録
	mNodeMap.emplace(node.mName, node.mIdx);// 検索用マップに登録
	// 再帰で子ノードを構築
	mSrcNodes[node.mIdx].mChildren.resize(srcNode->mNumChildren);
	for (uint32_t i = 0; i < srcNode->mNumChildren; ++i) {
		mSrcNodes[node.mIdx].mChildren[i] = CreateNode(srcNode->mChildren[i], node.mIdx);
	}
	return node.mIdx;
}

// デバッグ用出力
void Model::Output(Node* node, uint32_t depth) {
	Helper::Log(std::format("{}{}\n", std::string(depth, '\t'), node->mName));
	for (int32_t i : node->mChildren) {
		Output(&mSrcNodes[i], depth + 1);
	}
}

// メッシュの作成
void Model::CreateMesh(Node* node) {
	for (uint32_t meshIdx = 0; meshIdx < node->mAssimpNode->mNumMeshes; ++meshIdx) {
		aiMesh* srcMesh = mAssimpScene->mMeshes[node->mAssimpNode->mMeshes[meshIdx]];
		// 座標、法線、UV座標は必須
		if (!srcMesh->HasPositions() ||
			!srcMesh->HasNormals() ||
			!srcMesh->HasTextureCoords(0)) {
			continue;
		}

		std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
		mesh->mName = srcMesh->mName.C_Str();// メッシュ名
		mesh->mHasSkin = srcMesh->mNumBones > 0;// 関連ボーンがあればスキンあり

		// 頂点読み込み
		if (mesh->mHasSkin) {
			mesh->mSkinVertices.resize(srcMesh->mNumVertices);
		} else {
			mesh->mVertices.resize(srcMesh->mNumVertices);
		}
		for (uint32_t vertexIdx = 0; vertexIdx < srcMesh->mNumVertices; ++vertexIdx) {
			aiVector3D& position = srcMesh->mVertices[vertexIdx];
			aiVector3D& normal = srcMesh->mNormals[vertexIdx];
			aiVector3D& uv = srcMesh->mTextureCoords[0][vertexIdx];
			if (mesh->mHasSkin) {
				// 重みとボーンインデックスは後で
				mesh->mSkinVertices[vertexIdx] = Mesh::SkinVertex{
					Vector4(-position.x, position.y, position.z, 1.0f),
					Vector3(-normal.x, normal.y, normal.z),
					Vector2(uv.x, uv.y),
					{ 0.0f,0.0f,0.0f,0.0f },
					{ 0,0,0,0 }
				};
			} else {
				mesh->mVertices[vertexIdx] = Mesh::Vertex{
					Vector4(-position.x, position.y, position.z, 1.0f),
					Vector3(-normal.x, normal.y, normal.z),
					Vector2(uv.x, uv.y)
				};
			}
		}

		// インデックス読み込み
		for (uint32_t faceIdx = 0; faceIdx < srcMesh->mNumFaces; ++faceIdx) {
			aiFace& face = srcMesh->mFaces[faceIdx];
			if (face.mNumIndices > 4) {// 五角形以上の場合
				continue;// 無視
			}
			for (uint32_t i = 0; i < face.mNumIndices; ++i) {
				if (i == 3) {// 四角形の場合
					mesh->mIndices.emplace_back(face.mIndices[i - 1]);
					mesh->mIndices.emplace_back(face.mIndices[i]);
					mesh->mIndices.emplace_back(face.mIndices[i - 3]);
				} else {// 三角形の場合
					mesh->mIndices.emplace_back(face.mIndices[i]);
				}
			}
		}

		// ボーン読み込み
		if (mesh->mHasSkin) {
			// どの頂点にどれくらいの重みか
			struct WeightData {
				uint32_t mVertexIdx;
				float mWeight;
			};
			// このボーンが影響を与える頂点
			// 重みとボーンインデックスの設定用
			struct BoneData {
				std::vector<WeightData> mWeightData;
				uint32_t mBoneIdx;
			};
			std::unordered_map<std::string, BoneData> boneData = {};

			for (uint32_t boneIdx = 0; boneIdx < srcMesh->mNumBones; ++boneIdx) {
				aiBone* srcBone = srcMesh->mBones[boneIdx];
				std::string boneName = srcBone->mName.C_Str();// ボーン名

				// ボーン名と同じ名前のノードがあれば
				auto it = mNodeMap.find(boneName);
				if (it != mNodeMap.end()) {
					// ボーン用ノードの作成
					BoneNode boneNode = {};
					boneNode.mNodeIdx = it->second;// ノードと紐づける
					// 逆バインドポーズ行列を計算
					aiMatrix4x4 srcBindMat = srcBone->mOffsetMatrix.Inverse();
					aiVector3D scale, translate;
					aiQuaternion rotate;
					srcBindMat.Decompose(scale, rotate, translate);
					Matrix4 bindMat = CreateAffine4(
						Vector3(scale.x, scale.y, scale.z),
						Quaternion(rotate.x, -rotate.y, -rotate.z, rotate.w),
						Vector3(-translate.x, translate.y, translate.z)
					);
					boneNode.mInverseBindMat = Inverse(bindMat);
					mBones.emplace_back(std::move(boneNode));

					// このボーンが影響を与える頂点について
					boneData[boneName].mBoneIdx = static_cast<uint32_t>(mBones.size());
					for (uint32_t weightIdx = 0; weightIdx < srcBone->mNumWeights; ++weightIdx) {
						WeightData weightData = {};
						weightData.mVertexIdx = srcBone->mWeights[weightIdx].mVertexId;
						weightData.mWeight = srcBone->mWeights[weightIdx].mWeight;
						boneData[boneName].mWeightData.emplace_back(weightData);
					}
				}
			}

			// 重みとボーンインデックスを設定
			for (auto& bone : boneData) {
				for (WeightData& weightData : bone.second.mWeightData) {
					Mesh::SkinVertex& vertex = mesh->mSkinVertices[weightData.mVertexIdx];
					for (uint32_t i = 0; i < 4; ++i) {
						// まだ重みが設定されていなかったら
						if (vertex.mWeights[i] <= 0.0f) {
							vertex.mWeights[i] = weightData.mWeight;
							vertex.mBoneIndices[i] = bone.second.mBoneIdx;
							break;
						}
					}
				}
			}
		}

		// マテリアルリストのインデックスを設定
		mesh->mMaterialIdx = srcMesh->mMaterialIndex;

		// メッシュの作成と追加
		mesh->Create();
		MeshNode meshNode = {};
		meshNode.mNodeIdx = node->mIdx;// ノードと紐づける
		meshNode.mMesh = std::move(mesh);
		mMeshes.emplace_back(std::move(meshNode));
	}

	// 子ノードにくっついているメッシュも作成
	node->mChildren.resize(node->mChildren.size());
	for (uint32_t i = 0; i < node->mChildren.size(); ++i) {
		CreateMesh(&mSrcNodes[node->mChildren[i]]);
	}
}

// マテリアルの作成
void Model::CreateMaterial() {
	ResourceMgr& resourceMgr = ResourceMgr::GetInstance();

	for (uint32_t i = 0; i < mAssimpScene->mNumMaterials; ++i) {
		aiMaterial* srcMaterial = mAssimpScene->mMaterials[i];
		std::unique_ptr<Material> material = std::make_unique<Material>();

		aiString name;// マテリアル名
		srcMaterial->Get(AI_MATKEY_NAME, name);
		material->mName = name.C_Str();

		aiColor4D color;// 色
		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->mColor = Vector4(color.r, color.g, color.b, color.a);

		// テクスチャの読み込み
		Texture* texture = nullptr;
		if (srcMaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString texturePath;
			srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			// テクスチャはモデルファイルと同じディレクトリにある
			std::string parentPath = Helper::GetParentPath(mPath);
			std::string filename = Helper::GetFilename(texturePath.C_Str());
			texture = resourceMgr.GetTexture(parentPath + "/" + filename);
			if (!texture) {
				texture = resourceMgr.GetTexture("resources/error.png");
			}
		}
		material->mTexture = texture;

		// マテリアルの作成と追加
		material->Create();
		mSrcMaterials.emplace_back(std::move(material));
	}
}

// アニメーション読み込み
void Model::CreateAnimation() {
	// アニメーション用に読み込みなおす
	Assimp::Importer importer = {};
	const aiScene* scene = importer.ReadFile(mPath.c_str(), 0);
	if (scene->mNumAnimations <= 0) {
		return;
	}

	for (uint32_t animIdx = 0; animIdx != scene->mNumAnimations; ++animIdx) {
		aiAnimation* srcAnim = scene->mAnimations[animIdx];
		std::unique_ptr<Animation> anim = std::make_unique<Animation>();
		anim->mName = srcAnim->mName.C_Str();// アニメーション名
		// アニメーションの尺(秒に変換)
		anim->mDuration = static_cast<float>(srcAnim->mDuration / srcAnim->mTicksPerSecond);

		// ノードごとに読み込む
		for (uint32_t channelIdx = 0; channelIdx < srcAnim->mNumChannels; ++channelIdx) {
			aiNodeAnim* srcChannel = srcAnim->mChannels[channelIdx];
			NodeAnimation& nodeAnim = anim->mNodeAnimations[srcChannel->mNodeName.C_Str()];

			// スケール
			for (uint32_t keyIdx = 0; keyIdx < srcChannel->mNumScalingKeys; ++keyIdx) {
				aiVectorKey& srcKey = srcChannel->mScalingKeys[keyIdx];
				Keyframe<Vector3> keyframe = {};
				keyframe.mTime = static_cast<float>(srcKey.mTime / srcAnim->mTicksPerSecond);// 秒に変換
				keyframe.mValue = Vector3(srcKey.mValue.x, srcKey.mValue.y, srcKey.mValue.z);
				nodeAnim.mScale.mKeyframes.push_back(keyframe);
			}
			// 回転
			for (uint32_t keyIdx = 0; keyIdx < srcChannel->mNumRotationKeys; ++keyIdx) {
				aiQuatKey& srcKey = srcChannel->mRotationKeys[keyIdx];
				Keyframe<Quaternion> keyframe = {};
				keyframe.mTime = static_cast<float>(srcKey.mTime / srcAnim->mTicksPerSecond);
				keyframe.mValue = Quaternion(srcKey.mValue.x, -srcKey.mValue.y, -srcKey.mValue.z, srcKey.mValue.w);
				nodeAnim.mRotate.mKeyframes.emplace_back(keyframe);
			}
			// 座標
			for (uint32_t keyIdx = 0; keyIdx < srcChannel->mNumPositionKeys; ++keyIdx) {
				aiVectorKey& srcKey = srcChannel->mPositionKeys[keyIdx];
				Keyframe<Vector3> keyframe = {};
				keyframe.mTime = static_cast<float>(srcKey.mTime / srcAnim->mTicksPerSecond);
				keyframe.mValue = Vector3(-srcKey.mValue.x, srcKey.mValue.y, srcKey.mValue.z);
				nodeAnim.mTranslate.mKeyframes.emplace_back(keyframe);
			}
		}
		mSrcAnimations.emplace(anim->mName, std::move(anim));
	}

	importer.FreeScene();
}

// インスタンス化したモデル

void ModelInstance::Create(Model* src) {
	mSrc = src;
	if (mSrc) {
		// 元データからノードをコピー
		mNodes = mSrc->mSrcNodes;
		// メッシュ分、変換行列を作成
		mTransformationMatBuff.resize(mSrc->mMeshes.size());
		for (uint32_t i = 0; i < mSrc->mMeshes.size(); ++i) {
			mTransformationMatBuff[i] = std::make_unique<ConstantBuffer>();
			mTransformationMatBuff[i]->Create(sizeof(TransformationMatrix));
		}
		// 元データのマテリアルを使う
		mMaterials.resize(mSrc->mSrcMaterials.size());
		for (uint32_t i = 0; i < mSrc->mSrcMaterials.size(); ++i) {
			mMaterials[i] = mSrc->mSrcMaterials[i].get();
		}
		// ボーンがあれば行列パレットを作成
		if (mSrc->mBones.size() > 0) {
			mMatPaletteBuff = std::make_unique<StructuredBuffer>();
			mMatPaletteBuff->Create(static_cast<uint32_t>(mSrc->mBones.size()), sizeof(Matrix4));
			// ノードと行列パレットを更新しておく
			UpdateNode(mNodes.data());
			UpdatePalette();
		}
	}
}

// 更新(主にアニメーション)
void ModelInstance::Update(float deltaTime) {
	if (mCurrAnim) {
		mAnimTime += deltaTime * mPlayRate;
		if (mIsLoop) {
			mAnimTime = std::fmod(mAnimTime, mCurrAnim->mDuration);
		} else {
			if (mAnimTime > mCurrAnim->mDuration) {
				mAnimTime = mCurrAnim->mDuration;
			}
		}
		// アニメーションを更新
		auto& nodeAnims = mCurrAnim->mNodeAnimations;
		for (Model::Node& node : mNodes) {
			if (auto it = nodeAnims.find(node.mName); it != nodeAnims.end()) {
				NodeAnimation& nodeAnim = it->second;
				node.mScale = UpdateAnimation(nodeAnim.mScale, mAnimTime);
				node.mRotate = UpdateAnimation(nodeAnim.mRotate, mAnimTime);
				node.mTranslate = UpdateAnimation(nodeAnim.mTranslate, mAnimTime);
			}
		}
		// 全ノードを更新
		UpdateNode(mNodes.data());
		// 行列パレットを更新
		UpdatePalette();
	}
}

// 描画
void ModelInstance::Draw(const Matrix4& worldMat, ModelCamera* camera) {
	ModelBase& modelBase = ModelBase::GetInstance();

	// マテリアルを更新
	for (Material* material : mMaterials) {
		material->Update();
	}

	// メッシュごとに描画
	for (uint32_t i = 0; i < mSrc->mMeshes.size(); ++i) {
		Mesh* mesh = mSrc->mMeshes[i].mMesh.get();// メッシュ
		Material* material = mMaterials[mesh->mMaterialIdx];// マテリアル

		// 変換行列を更新
		if (!camera) {
			camera = modelBase.GetDefaultCamera();
		}
		TransformationMatrix data = {};
		data.mWorldMat = mNodes[mSrc->mMeshes[i].mNodeIdx].mWorldMat * worldMat;
		data.mWVPMat = data.mWorldMat * camera->GetViewProjectionMat();
		data.mWorldInverseTransposeMat = Transpose(Inverse(data.mWorldMat));
		mTransformationMatBuff[i]->Update(data);

		// 描画コマンド
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = DirectXBase::GetInstance().GetCmdList();
		PSOFlags pso = PSOFlags::kNone;
		if (material->mTexture) {
			pso |= PSOFlags::kTexture;
			material->mTexture->Set(cmdList, static_cast<uint32_t>(ModelBase::RootParamNum::kTexture));
		} else {
			mFlags &= ~PSOFlags::kTexture;
		}
		if (mesh->mHasSkin) {
			pso |= PSOFlags::kSkin;
			mMatPaletteBuff->Set(cmdList, static_cast<uint32_t>(ModelBase::RootParamNum::kMatPalette));
		} else {
			mFlags &= ~PSOFlags::kSkin;
		}
		modelBase.SetPipelineState(mFlags | pso);
		mTransformationMatBuff[i]->Set(cmdList, static_cast<uint32_t>(ModelBase::RootParamNum::kTransformationMat));
		material->Set(cmdList, static_cast<uint32_t>(ModelBase::RootParamNum::kMaterial));
		camera->Set(cmdList, static_cast<uint32_t>(ModelBase::RootParamNum::kCamera));
		mesh->Draw(cmdList);// 描画
	}
}

// アニメーション名で再生
void ModelInstance::PlayAtName(const std::string& name, float rate) {
	auto it = mSrc->mSrcAnimations.find(name);
	if (it != mSrc->mSrcAnimations.end()) {
		mCurrAnim = std::make_unique<Animation>();
		*mCurrAnim = *it->second.get();
		mPlayRate = rate;
		mAnimTime = 0.0f;
	}
}

// アニメーションのインデックスで再生
void ModelInstance::PlayAtIdx(uint32_t idx, float rate) {
	assert(idx >= 0 && idx < mSrc->mSrcAnimations.size());
	auto it = std::next(mSrc->mSrcAnimations.begin(), idx);
	if (it != mSrc->mSrcAnimations.end()) {
		mCurrAnim = std::make_unique<Animation>();
		*mCurrAnim = *it->second.get();
		mPlayRate = rate;
		mAnimTime = 0.0f;
	}
}

// マテリアルをセット
void ModelInstance::SetMaterial(uint32_t idx, Material* material) {
	assert(idx >= 0 && idx < mMaterials.size());
	mMaterials[idx] = material;
}

// 再帰で全ノードを更新
void ModelInstance::UpdateNode(Model::Node* node) {
	node->mLocalMat = CreateAffine4(node->mScale, node->mRotate, node->mTranslate);
	if (node->mParent) {
		node->mWorldMat = node->mLocalMat * mNodes[*node->mParent].mWorldMat;
	} else {
		node->mWorldMat = node->mLocalMat;
	}
	for (int32_t& i : node->mChildren) {
		UpdateNode(&mNodes[i]);
	}
}

// 行列パレットを更新
void ModelInstance::UpdatePalette() {
	std::vector<Matrix4> matPalette(mSrc->mBones.size());
	for (uint32_t i = 0; i < matPalette.size(); ++i) {
		// 逆バインドポーズ行列*ボーンの行列
		matPalette[i] = mSrc->mBones[i].mInverseBindMat * mNodes[mSrc->mBones[i].mNodeIdx].mWorldMat;
	}
	mMatPaletteBuff->Update(matPalette.data());
}

// アニメーション更新
Vector3 ModelInstance::UpdateAnimation(const AnimationCurve<Vector3>& animCurve, float time) {
	assert(!animCurve.mKeyframes.empty());
	if (animCurve.mKeyframes.size() == 1 || time <= animCurve.mKeyframes[0].mTime) {
		return animCurve.mKeyframes[0].mValue;
	}
	for (uint32_t currIdx = 0; currIdx < animCurve.mKeyframes.size() - 1; ++currIdx) {
		uint32_t nextIdx = currIdx + 1;
		float currTime = animCurve.mKeyframes[currIdx].mTime;
		float nextTime = animCurve.mKeyframes[nextIdx].mTime;
		if (currTime <= time && nextTime >= time) {
			float t = (time - currTime) / (nextTime - currTime);
			return animCurve.mKeyframes[currIdx].mValue * (1.0f - t) + animCurve.mKeyframes[nextIdx].mValue * t;
		}
	}
	return (*animCurve.mKeyframes.rbegin()).mValue;
}
Quaternion ModelInstance::UpdateAnimation(const AnimationCurve<Quaternion>& animCurve, float time) {
	assert(!animCurve.mKeyframes.empty());
	if (animCurve.mKeyframes.size() == 1 || time <= animCurve.mKeyframes[0].mTime) {
		return animCurve.mKeyframes[0].mValue;
	}
	for (uint32_t currIdx = 0; currIdx < animCurve.mKeyframes.size() - 1; ++currIdx) {
		uint32_t nextIdx = currIdx + 1;
		float currTime = animCurve.mKeyframes[currIdx].mTime;
		float nextTime = animCurve.mKeyframes[nextIdx].mTime;
		if (currTime <= time && nextTime >= time) {
			float t = (time - currTime) / (nextTime - currTime);
			return Slerp(animCurve.mKeyframes[currIdx].mValue, animCurve.mKeyframes[nextIdx].mValue, t);
		}
	}
	return (*animCurve.mKeyframes.rbegin()).mValue;
}
