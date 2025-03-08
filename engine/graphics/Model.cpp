#include "Model.h"
#include "directX/DirectXBase.h"
#include "ModelBase.h"
#include "Helper.h"
#include "ModelCamera.h"
#include "ResourceMgr.h"
#include <cassert>
#include <format>

void Material::Create() {
	mConstantBuffer = std::make_unique<ConstantBuffer>();
	mConstantBuffer->Create(sizeof(Constant));
	Update();
}

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
	mConstantBuffer->Update(&data);
}

void Material::Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, uint32_t rootParamIdx) {
	mConstantBuffer->Set(cmdList, rootParamIdx);
}

void Mesh::Create() {
	if (mHasSkin) {
		mVertexBuffer = std::make_unique<VertexBuffer>();
		mVertexBuffer->Create(static_cast<uint32_t>(sizeof(SkinVertex) * mSkinVertices.size()), sizeof(SkinVertex), mSkinVertices.data());
	} else {
		mVertexBuffer = std::make_unique<VertexBuffer>();
		mVertexBuffer->Create(static_cast<uint32_t>(sizeof(Vertex) * mVertices.size()), sizeof(Vertex), mVertices.data());
	}
	mIndexBuffer = std::make_unique<IndexBuffer>();
	mIndexBuffer->Create(sizeof(uint32_t) * static_cast<uint32_t>(mIndices.size()), mIndices.data());
}

void Mesh::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	mVertexBuffer->Set(cmdList);
	mIndexBuffer->Set(cmdList);
	cmdList->DrawIndexedInstanced(static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
}

bool ModelData::Create(const std::string& path) {
	mPath = path;
	Assimp::Importer importer = {};
	mAssimpScene = importer.ReadFile(path.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	if (!mAssimpScene->HasMeshes()) {
		return false;
	}

	mRootIndex = CreateNode(mAssimpScene->mRootNode, {});
	bool isOutput = false;
	if (isOutput) {
		Helper::Log(std::format("'{}'\n", mPath));
		Output(&mSrcNodes[mRootIndex]);
	}

	CreateMesh(&mSrcNodes[mRootIndex]);
	CreateMaterial();
	CreateAnimation();

	Helper::Log(std::format("Create '{}'\n", mPath));
	return true;
}

int32_t ModelData::CreateNode(aiNode* srcNode, const std::optional<int32_t>& parent) {
	Node node = {};
	node.mAssimpNode = srcNode;
	node.mName = srcNode->mName.C_Str();
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
	node.mIndex = static_cast<uint32_t>(mSrcNodes.size());
	node.mParent = parent;
	mSrcNodes.emplace_back(node);
	mNodeMap.emplace(node.mName, node.mIndex);

	mSrcNodes[node.mIndex].mChildren.resize(srcNode->mNumChildren);
	for (uint32_t i = 0; i < srcNode->mNumChildren; ++i) {
		mSrcNodes[node.mIndex].mChildren[i] = CreateNode(srcNode->mChildren[i], node.mIndex);
	}
	return node.mIndex;
}

void ModelData::Output(Node* node, uint32_t depth) {
	Helper::Log(std::format("{}{}\n", std::string(depth, '\t'), node->mName));
	for (int32_t i : node->mChildren) {
		Output(&mSrcNodes[i], depth + 1);
	}
}

void ModelData::CreateMesh(Node* node) {
	for (uint32_t meshIdx = 0; meshIdx < node->mAssimpNode->mNumMeshes; ++meshIdx) {
		aiMesh* srcMesh = mAssimpScene->mMeshes[node->mAssimpNode->mMeshes[meshIdx]];
		if (!srcMesh->HasPositions() ||
			!srcMesh->HasNormals() ||
			!srcMesh->HasTextureCoords(0)) {
			continue;
		}

		std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
		mesh->mName = srcMesh->mName.C_Str();
		mesh->mHasSkin = srcMesh->mNumBones > 0;

		if (mesh->mHasSkin) {
			mesh->mSkinVertices.resize(srcMesh->mNumVertices);
		} else {
			mesh->mVertices.resize(srcMesh->mNumVertices);
		}
		for (uint32_t vertIdx = 0; vertIdx < srcMesh->mNumVertices; ++vertIdx) {
			aiVector3D& position = srcMesh->mVertices[vertIdx];
			aiVector3D& normal = srcMesh->mNormals[vertIdx];
			aiVector3D& uv = srcMesh->mTextureCoords[0][vertIdx];
			if (mesh->mHasSkin) {
				mesh->mSkinVertices[vertIdx] = Mesh::SkinVertex{
					Vector4(-position.x, position.y, position.z, 1.0f),
					Vector3(-normal.x, normal.y, normal.z),
					Vector2(uv.x, uv.y),
					{ 0.0f,0.0f,0.0f,0.0f },
					{ 0,0,0,0 }
				};
			} else {
				mesh->mVertices[vertIdx] = Mesh::Vertex{
					Vector4(-position.x, position.y, position.z, 1.0f),
					Vector3(-normal.x, normal.y, normal.z),
					Vector2(uv.x, uv.y)
				};
			}
		}

		for (uint32_t faceIdx = 0; faceIdx < srcMesh->mNumFaces; ++faceIdx) {
			aiFace& face = srcMesh->mFaces[faceIdx];
			if (face.mNumIndices > 4) {
				continue;
			}
			for (uint32_t i = 0; i < face.mNumIndices; ++i) {
				if (i == 3) {
					mesh->mIndices.emplace_back(face.mIndices[i - 1]);
					mesh->mIndices.emplace_back(face.mIndices[i]);
					mesh->mIndices.emplace_back(face.mIndices[i - 3]);
				} else {
					mesh->mIndices.emplace_back(face.mIndices[i]);
				}
			}
		}

		struct WeightData {
			uint32_t mVertexId;
			float mWeight;
		};
		struct BoneData {
			uint32_t mBoneIndex;
			std::vector<WeightData> mWeights;
		};
		std::unordered_map<std::string, BoneData> boneData = {};
		for (uint32_t boneIdx = 0; boneIdx < srcMesh->mNumBones; ++boneIdx) {
			aiBone* srcBone = srcMesh->mBones[boneIdx];
			auto it = mNodeMap.find(srcBone->mName.C_Str());
			if (it != mNodeMap.end()) {
				BoneNode boneNode = {};
				boneNode.mNodeIndex = it->second;
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

				BoneData& bone = boneData[srcBone->mName.C_Str()];
				bone.mBoneIndex = static_cast<uint32_t>(mBones.size());
				for (uint32_t weightIdx = 0; weightIdx < srcBone->mNumWeights; ++weightIdx) {
					WeightData weight = {};
					weight.mVertexId = srcBone->mWeights[weightIdx].mVertexId;
					weight.mWeight = srcBone->mWeights[weightIdx].mWeight;
					bone.mWeights.emplace_back(weight);
				}
			}
		}

		if (mesh->mHasSkin) {
			for (auto& bone : boneData) {
				for (WeightData& weightData : bone.second.mWeights) {
					Mesh::SkinVertex& vertex = mesh->mSkinVertices[weightData.mVertexId];
					for (uint32_t i = 0; i < 4; ++i) {
						if (vertex.mWeights[i] <= 0.0f) {
							vertex.mWeights[i] = weightData.mWeight;
							vertex.mBoneIndices[i] = bone.second.mBoneIndex;
							break;
						}
					}
				}
			}
		}

		mesh->mMaterialIndex = srcMesh->mMaterialIndex;
		mesh->Create();
		MeshNode meshNode = {};
		meshNode.mNodeIndex = node->mIndex;
		meshNode.mMesh = std::move(mesh);
		mMeshes.emplace_back(std::move(meshNode));
	}

	node->mChildren.resize(node->mChildren.size());
	for (uint32_t i = 0; i < node->mChildren.size(); ++i) {
		CreateMesh(&mSrcNodes[node->mChildren[i]]);
	}
}

void ModelData::CreateMaterial() {
	ResourceMgr& resourceMgr = ResourceMgr::GetInstance();
	for (uint32_t i = 0; i < mAssimpScene->mNumMaterials; ++i) {
		aiMaterial* srcMaterial = mAssimpScene->mMaterials[i];
		std::unique_ptr<Material> material = std::make_unique<Material>();
		aiString name;
		srcMaterial->Get(AI_MATKEY_NAME, name);
		material->mName = name.C_Str();
		aiColor4D color;
		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->mColor = Vector4(color.r, color.g, color.b, color.a);
		Texture* texture = nullptr;
		if (srcMaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString texturePath;
			srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			std::string parentPath = Helper::GetParentPath(mPath);
			std::string filename = Helper::GetFilename(texturePath.C_Str());
			texture = resourceMgr.GetTexture(parentPath + "/" + filename);
		}
		if (!texture) {
			texture = resourceMgr.GetTexture("resources/error.png");
		}
		material->mTexture = texture;
		material->Create();
		mSrcMaterials.emplace_back(std::move(material));
	}
}

void ModelData::CreateAnimation() {
	Assimp::Importer importer = {};
	const aiScene* scene = importer.ReadFile(mPath.c_str(), 0);
	if (scene->mNumAnimations <= 0) {
		return;
	}
	for (uint32_t animIdx = 0; animIdx != scene->mNumAnimations; ++animIdx) {
		aiAnimation* srcAnim = scene->mAnimations[animIdx];
		std::unique_ptr<Animation> anim = std::make_unique<Animation>();
		anim->mName = srcAnim->mName.C_Str();
		anim->mDuration = static_cast<float>(srcAnim->mDuration / srcAnim->mTicksPerSecond);

		for (uint32_t channelIdx = 0; channelIdx < srcAnim->mNumChannels; ++channelIdx) {
			aiNodeAnim* srcChannel = srcAnim->mChannels[channelIdx];
			NodeAnimation& channel = anim->mNodeAnimations[srcChannel->mNodeName.C_Str()];
			for (uint32_t keyIdx = 0; keyIdx < srcChannel->mNumScalingKeys; ++keyIdx) {
				aiVectorKey& srcKeys = srcChannel->mScalingKeys[keyIdx];
				Keyframe<Vector3> keyframe = {};
				keyframe.mTime = static_cast<float>(srcKeys.mTime / srcAnim->mTicksPerSecond);
				keyframe.mValue = Vector3(srcKeys.mValue.x, srcKeys.mValue.y, srcKeys.mValue.z);
				channel.mScale.mKeyframes.push_back(keyframe);
			}
			for (uint32_t keyIdx = 0; keyIdx < srcChannel->mNumRotationKeys; ++keyIdx) {
				aiQuatKey& srcKeys = srcChannel->mRotationKeys[keyIdx];
				Keyframe<Quaternion> keyframe = {};
				keyframe.mTime = static_cast<float>(srcKeys.mTime / srcAnim->mTicksPerSecond);
				keyframe.mValue = Quaternion(srcKeys.mValue.x, -srcKeys.mValue.y, -srcKeys.mValue.z, srcKeys.mValue.w);
				channel.mRotate.mKeyframes.emplace_back(keyframe);
			}
			for (uint32_t keyIdx = 0; keyIdx < srcChannel->mNumPositionKeys; ++keyIdx) {
				aiVectorKey& srcKeys = srcChannel->mPositionKeys[keyIdx];
				Keyframe<Vector3> keyframe = {};
				keyframe.mTime = static_cast<float>(srcKeys.mTime / srcAnim->mTicksPerSecond);
				keyframe.mValue = Vector3(-srcKeys.mValue.x, srcKeys.mValue.y, srcKeys.mValue.z);
				channel.mTranslate.mKeyframes.emplace_back(keyframe);
			}
		}
		mSrcAnimations.emplace(anim->mName, std::move(anim));
	}
}

void Model::Create(ModelData* model) {
	mSrcModel = model;
	mNodes = mSrcModel->mSrcNodes;

	mTransformBuff.resize(mSrcModel->mMeshes.size());
	for (uint32_t i = 0; i < mSrcModel->mMeshes.size(); ++i) {
		mTransformBuff[i] = std::make_unique<ConstantBuffer>();
		mTransformBuff[i]->Create(sizeof(TransformationMatrix));
	}

	mMaterials.resize(mSrcModel->mSrcMaterials.size());
	for (uint32_t i = 0; i < mSrcModel->mSrcMaterials.size(); ++i) {
		mMaterials[i] = mSrcModel->mSrcMaterials[i].get();
	}

	if (mSrcModel->mBones.size() > 0) {
		mPaletteBuff = std::make_unique<StructuredBuffer>();
		mPaletteBuff->Create(static_cast<uint32_t>(mSrcModel->mBones.size()), sizeof(Matrix4));
		UpdateNode(mNodes.data());
		UpdatePalette();
	}
}

void Model::Update(float deltaTime) {
	if (mAnimation) {
		mAnimationTime += deltaTime * mPlayRate;
		if (mIsLoop) {
			mAnimationTime = std::fmod(mAnimationTime, mAnimation->mDuration);
		} else {
			if (mAnimationTime > mAnimation->mDuration) {
				mAnimationTime = mAnimation->mDuration;
			}
		}
		auto& animations = mAnimation->mNodeAnimations;
		for (ModelData::Node& node : mNodes) {
			if (auto it = animations.find(node.mName); it != animations.end()) {
				NodeAnimation& nodeAnimation = it->second;
				node.mScale = UpdateAnimation(nodeAnimation.mScale, mAnimationTime);
				node.mRotate = UpdateAnimation(nodeAnimation.mRotate, mAnimationTime);
				node.mTranslate = UpdateAnimation(nodeAnimation.mTranslate, mAnimationTime);
			}
		}
		UpdateNode(mNodes.data());
		UpdatePalette();
	}
}

void Model::Draw(const Matrix4& worldMat, ModelCamera* camera) {
	ModelBase& modelBase = ModelBase::GetInstance();
	for (auto material : mMaterials) {
		material->Update();
	}

	for (uint32_t i = 0; i < mSrcModel->mMeshes.size(); ++i) {
		ModelData::MeshNode& meshNode = mSrcModel->mMeshes[i];
		Mesh* mesh = meshNode.mMesh.get();
		Material* material = mMaterials[mesh->mMaterialIndex];

		if (!camera) {
			camera = modelBase.GetDefaultCamera();
		}
		TransformationMatrix data = {};
		data.mWorldMat = mNodes[meshNode.mNodeIndex].mWorldMat * worldMat;
		data.mWVPMat = data.mWorldMat * camera->GetViewProjectionMat();
		data.mWorldInverseTransposeMat = Transpose(Inverse(data.mWorldMat));
		mTransformBuff[i]->Update(data);

		PSOFlags flags = PSOFlags::kNone;
		if (!material->mTexture) {
			flags |= PSOFlags::kHasTexture;
		}
		if (mesh->mHasSkin) {
			flags |= PSOFlags::kHasSkin;
		}
		modelBase.SetPipelineState(mFlags | flags);

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList = DirectXBase::GetInstance().GetCmdList();
		mTransformBuff[i]->Set(cmdList, 0);
		material->Bind(cmdList, 1);
		if (material->mTexture) {
			material->mTexture->Set(cmdList, 2);
		}
		camera->Set(cmdList, 3);
		if (mesh->mHasSkin) {
			mPaletteBuff->Set(cmdList, 8);
		}
		mesh->Draw(cmdList);
	}
}

void Model::Play(const std::string& animationName, float rate) {
	auto it = mSrcModel->mSrcAnimations.find(animationName);
	if (it != mSrcModel->mSrcAnimations.end()) {
		mAnimation = std::make_unique<Animation>();
		*mAnimation = *it->second.get();
		mAnimationTime = 0.0f;
		mPlayRate = rate;
	}
}

void Model::SetMaterial(uint32_t idx, Material* material) {
	assert(idx >= 0 && idx < mMaterials.size());
	mMaterials[idx] = material;
}

void Model::UpdateNode(ModelData::Node* node) {
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

void Model::UpdatePalette() {
	std::vector<Matrix4> palette(mSrcModel->mBones.size());
	for (uint32_t i = 0; i < palette.size(); ++i) {
		palette[i] =
			mSrcModel->mBones[i].mInverseBindMat *
			mNodes[mSrcModel->mBones[i].mNodeIndex].mWorldMat;
	}
	mPaletteBuff->Update(palette.data());
}

Vector3 Model::UpdateAnimation(const AnimationCurve<Vector3>& animCurve, float time) {
	assert(!animCurve.mKeyframes.empty());
	if (animCurve.mKeyframes.size() == 1 || time <= animCurve.mKeyframes[0].mTime) {
		return animCurve.mKeyframes[0].mValue;
	}
	for (uint32_t curr = 0; curr < animCurve.mKeyframes.size() - 1; ++curr) {
		uint32_t next = curr + 1;
		float currTime = animCurve.mKeyframes[curr].mTime;
		float nextTime = animCurve.mKeyframes[next].mTime;
		if (currTime <= time && nextTime >= time) {
			float t = (time - currTime) / (nextTime - currTime);
			return animCurve.mKeyframes[curr].mValue * (1.0f - t) + animCurve.mKeyframes[next].mValue * t;
		}
	}
	return (*animCurve.mKeyframes.rbegin()).mValue;
}

Quaternion Model::UpdateAnimation(const AnimationCurve<Quaternion>& animCurve, float time) {
	assert(!animCurve.mKeyframes.empty());
	if (animCurve.mKeyframes.size() == 1 || time <= animCurve.mKeyframes[0].mTime) {
		return animCurve.mKeyframes[0].mValue;
	}
	for (uint32_t curr = 0; curr < animCurve.mKeyframes.size() - 1; ++curr) {
		uint32_t next = curr + 1;
		float currTime = animCurve.mKeyframes[curr].mTime;
		float nextTime = animCurve.mKeyframes[next].mTime;
		if (currTime <= time && nextTime >= time) {
			float t = (time - currTime) / (nextTime - currTime);
			return Slerp(animCurve.mKeyframes[curr].mValue, animCurve.mKeyframes[next].mValue, t);
		}
	}
	return (*animCurve.mKeyframes.rbegin()).mValue;
}
