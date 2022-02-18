#pragma once
#include <string>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <Shlwapi.h>
#include <filesystem>
#include <fbxsdk.h>
#include <fstream>


#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/bitset.hpp>


#include "misc.h"
using namespace fbxsdk;

#include "MyMath.h"
#include "DirectXSystem.h"
#include "Texture.h"
#define MAXBONEINFLUENCE 4
#define MAXBONES 128


namespace StartingOver
{
	struct Vertex
	{
		Vector3 position = {};
		Vector3 normal = {};
		Vector4 tangent = {};
		Vector3 biNormal = {};
		Vector2 texCoord = {};
		Vector4 color = { 0,0,1,1 };
		float boneWeight[MAXBONEINFLUENCE] = { 1,0,0,0 };
		std::uint32_t boneIndices[MAXBONEINFLUENCE] = {};

	};


	struct HierarchyNode
	{
		FbxNodeAttribute::EType attribute;
		std::string name;
		std::vector<HierarchyNode> children;

		friend class cereal::access;
		template<class T>
		void serialize(T& archive)
		{
			archive(attribute, name, children);
		}
	};

	struct AnimationTake
	{
		std::string name;
		size_t samplingRate = 0;
		size_t keyframeSize = 0;

		friend class cereal::access;
		template<class T>
		void serialize(T& archive)
		{
			archive(name, samplingRate, keyframeSize);
		}

	};

	inline void FbxAMatrixToMatrix(const FbxAMatrix& fbxAMatrix, Matrix& matrix)
	{
		for (int row = 0; row < 4; row++)
		{
			for (int column = 0; column < 4; column++)
			{
				matrix.m[row][column] = static_cast<float>(fbxAMatrix[row][column]);
			}
		}
	}
	void FetchAnimationTakes(FbxScene* scene, std::vector<AnimationTake>& animationTakes, size_t samplingRate = 0);

	struct Bone
	{
		std::string name;

		Matrix transform; // ボーン空間からグローバル空間
		Matrix transformToParent; // ボーン空間から親ボーン空間

		friend class cereal::access;
		template<class T>
		void serialize(T& archive)
		{
			archive
			(
				name,
				transform,
				transformToParent
			);
		}
	};

	struct Skeletal
	{
		std::vector<Bone> bones;
		const Bone* FindBone(const std::string& boneName) const
		{
			for (const Bone& bone : bones)
			{
				if (bone.name == boneName)
				{
					return &bone;
				}
			}
			return 0;
		}

		friend class cereal::access;
		template<class T>
		void serialize(T& archive)
		{
			archive(bones);
		}
	};

	struct ActorTransform
	{
		Matrix transform; //  モデル空間からグローバル空間

		friend class cereal::access;
		template<class T>
		void serialize(T& archive)
		{
			archive
			(
				//name,
				transform
			);
		}
	};

	struct Animation
	{
		std::string name;
		std::vector<Skeletal> skeletalTransforms;
		std::vector<ActorTransform> actorTransforms;

		friend class cereal::access;
		template<class T>
		void serialize(T& archive)
		{
			archive(name, skeletalTransforms, actorTransforms);
		}
	};

	void FetchAnimation(FbxMesh* fbxMesh, const AnimationTake& animationTake, Animation& animation);
	void FetchOffsetMatrices(const FbxMesh* fbxMesh, std::vector<Bone>& offsetTransforms);

	struct Material
	{
		std::string name;
		std::string shadingModel;
		bool multiLayer = false;

		struct Property
		{
			std::string name;

			Vector4 color = { 0.0f, 0.0f, 0.0f, 0.0f }; // w channel is used as shininess by only specular.
			float factor = 1.0f;

			std::string textureFileName;
			float uScale = 1.0f;
			float vScale = 1.0f;

			bool forceSRGB = false;

			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRView;

			friend class cereal::access;
			template<class T>
			void serialize(T& archive)
			{
				archive
				(
					name,
					color,
					factor,
					textureFileName,
					uScale, vScale, forceSRGB
				);
			}
		};
		std::vector<Property> properties;

		friend class cereal::access;
		template<class T>
		void serialize(T& archive)
		{
			archive(name, shadingModel, multiLayer, properties);
		}
	};

	void FetchMaterials(const FbxNode* fbxNode, std::unordered_map<std::string, Material>& materials);

	struct BoundingBox
	{
		Vector3 min{ +FLT_MAX,+FLT_MAX,+FLT_MAX };
		Vector3 max{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

		friend class cereal::access;
		template<class T>
		void serialize(T& archive)
		{
			archive(min, max);
		}

	};

	struct Subset
	{
		std::string name; // マテリアル名
		size_t indexStart = 0; // インデックスバッファの開始番号
		size_t indexCount = 0; // 頂点の数（インデックス)

		friend class cereal::access;
		template<class T>
		void serialize(T& archive)
		{
			archive(name, indexStart, indexCount);
		}
	};
	void FetchVerticesAndIndices(FbxMesh* fbxMesh, std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices, std::vector<Subset>& subsets);

	template <class T>
	struct Mesh
	{
		std::string name;

		std::vector<T> vertices;
		std::vector<std::uint32_t> indices;

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;

		std::vector<Subset> subsets;

		std::string FindMaterial(size_t indexOfTriangle)
		{
			const size_t indexOfIndices = 3 * indexOfTriangle;
			for (const Subset& subset : subsets)
			{
				if (subset.indexStart <= indexOfIndices &&
					subset.indexStart + subset.indexCount > indexOfIndices)
				{
					if (subset.name.empty())
					{
						return "";
					}
					else
					{
						return subset.name.c_str();
					}
				}
			}

			return nullptr;
		}

		std::vector<Animation> animations;
		std::vector<Bone> offsetTransforms;

		const Animation* FindAnimation(const std::string animationName)
		{
			for (const Animation& animation : animations)
			{
				if (animation.name == animationName)
				{
					return &animation;
				}
			}
			return nullptr;
		}

		Matrix globalTransform = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 };

		BoundingBox boundingBox;

		friend class cereal::access;
		template<class T>
		void serialize(T& archive)
		{
			archive
			(
				name, vertices, indices, subsets,
				globalTransform,
				animations, offsetTransforms, boundingBox
			);
		}

	};

	template <class T>
	struct Substance
	{
	public:
		Substance() = default;
		virtual ~Substance() = default;
		Substance(Substance&) = delete;
		Substance& operator=(Substance&) = delete;
		Substance(const std::string fbxFileName/* ファイルネームとパス */, bool triangulate, int animationSamplingRate = 0, const std::string extension = ".bin")
		{
			fileName = fbxFileName + extension.c_str();
			if (PathFileExistsA((std::string(fbxFileName) + extension).c_str()))
			{
				// 読み込み
				std::ifstream stream;
				stream.open(std::string(fbxFileName + extension.c_str()), std::ios::binary);
				cereal::BinaryInputArchive archive(stream);
				archive(*this);
			}
			else
			{
				// fbxファイルをシリアライズする
				CreateFromFbx(fbxFileName, triangulate, animationSamplingRate);

				std::ofstream stream;
				stream.open(std::string(fbxFileName + extension.c_str()), std::ios::binary);
				cereal::BinaryOutputArchive archive(stream);
				archive(*this);
			}
		}
		Substance(const std::string fbxFileName/* ファイルネームとパス */, bool triangulate, const std::string animationFileNames[], size_t animationNumSize, int animationSamplingRate = 0, const std::string extension = ".bin")
		{
			fileName = fbxFileName + extension.c_str();

			if (PathFileExistsA((std::string(fbxFileName) + extension).c_str()))
			{
				std::ifstream stream;
				stream.open((std::string(fbxFileName) + extension).c_str(), std::ios::binary);
				cereal::BinaryInputArchive archive(stream);
				archive(*this);
			}
			else
			{
				CreateFromFbx(fbxFileName, triangulate, animationSamplingRate);
				/*for(size_t i =0; _animationFileNames && i < _animationNumSize;i++)
				{
					AddAnimations(_animationFileNames[i], _animationSamplingRate);
				}*/

				std::ofstream stream;
				stream.open((std::string(fbxFileName) + extension).c_str(), std::ios::binary);
				cereal::BinaryOutputArchive archive(stream);
				archive(*this);

			}
		}


		void SetColor(const std::string materialName, const int propertyNum = 0, const Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f))
		{
			// TODO lambert2は変える
			// とりあえず仮
			materials.at("lambert2").properties.at(propertyNum).color = color;
		}
	
		void CreateFromFbx(const std::string fbxFileName, bool triangulate, int animationSamplingRate)
		{
			// Create FbxManager
			FbxManager* fbxManager = FbxManager::Create();

			fbxManager->SetIOSettings(FbxIOSettings::Create(fbxManager, IOSROOT));

			FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

			bool importStatus = fbxImporter->Initialize(fbxFileName.c_str(), -1, fbxManager->GetIOSettings());
			_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus(), GetErrorString());

			FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

			importStatus = fbxImporter->Import(fbxScene);
			_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus(), GetErrorString());

			// 左手座標系に変換 (Y-UP Z-FRONT)
			const FbxAxisSystem axisSystemReference = fbxScene->GetGlobalSettings().GetAxisSystem();
			const FbxAxisSystem::ECoordSystem coordSystem = axisSystemReference.GetCoorSystem();

			int upVectorSign = 1;
			const FbxAxisSystem::EUpVector upVector = axisSystemReference.GetUpVector(upVectorSign);
			_ASSERT_EXPR(upVector == FbxAxisSystem::eYAxis || upVector == FbxAxisSystem::eZAxis, L"Not support X-Up axis system");

			if (upVector == FbxAxisSystem::eYAxis)
			{
				axisSystemTransform = Matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
				if (coordSystem == FbxAxisSystem::eRightHanded)
				{
					axisSystemTransform._11 = -1;
				}
				else if (coordSystem == FbxAxisSystem::eLeftHanded)
				{
				}

			}
			else if (upVector == FbxAxisSystem::eZAxis)
			{
				axisSystemTransform = Matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
				if (coordSystem == FbxAxisSystem::eRightHanded)
				{
					axisSystemTransform._11 = -1;
					axisSystemTransform._23 = -1;
				}
				else if (coordSystem == FbxAxisSystem::eLeftHanded)
				{
					axisSystemTransform._11 = -1;
				}
			}

			if (fbxScene->GetGlobalSettings().GetSystemUnit() == FbxSystemUnit::cm)
			{
				systemUnitTransform._11 = systemUnitTransform._22 = systemUnitTransform._33 = 0.01f;
			}

			if (triangulate)
			{
				FbxGeometryConverter geometryConverter(fbxManager);
				geometryConverter.Triangulate(fbxScene, true);
			}

			if (animationSamplingRate > -1)
			{
				FetchAnimationTakes(fbxScene, animationTakes, animationSamplingRate);
			}

			std::vector<FbxNode*> fetchedMeshes;
			std::function<void(FbxNode*)> Traverse = [&](FbxNode* node)
			{
				FetchMaterials(node, materials);
				if (const auto fbxNodeAttribute = node->GetNodeAttribute())
				{
					switch (fbxNodeAttribute->GetAttributeType())
					{
					case FbxNodeAttribute::eMesh:
						fetchedMeshes.push_back(node);
						break;
					default:
						break;
					}
				}
				for (int i = 0; i < node->GetChildCount(); i++)
				{
					Traverse(node->GetChild(i));
				}
			};

			Traverse(fbxScene->GetRootNode());

			meshes.resize(fetchedMeshes.size());
			for (size_t i = 0; i < fetchedMeshes.size(); i++)
			{
				auto fbxMesh = fetchedMeshes.at(i)->GetMesh();

				auto& mesh = meshes.at(i);
				mesh.name = fbxMesh->GetNode()->GetName();

				auto& globalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
				FbxAMatrixToMatrix(globalTransform, mesh.globalTransform);

				std::vector<Vertex> vertices;
				FetchVerticesAndIndices(fbxMesh, vertices, mesh.indices, mesh.subsets);

				for (const auto& v : vertices)
				{
					mesh.vertices.push_back(v);
					mesh.boundingBox.max.x = std::max<float>(v.position.x, mesh.boundingBox.max.x);
					mesh.boundingBox.max.y = std::max<float>(v.position.y, mesh.boundingBox.max.y);
					mesh.boundingBox.max.z = std::max<float>(v.position.z, mesh.boundingBox.max.z);
					mesh.boundingBox.min.x = std::min<float>(v.position.x, mesh.boundingBox.min.x);
					mesh.boundingBox.min.y = std::min<float>(v.position.y, mesh.boundingBox.min.y);
					mesh.boundingBox.min.z = std::min<float>(v.position.z, mesh.boundingBox.min.z);
				}

				FetchOffsetMatrices(fbxMesh, mesh.offsetTransforms);
				for (const auto animationTake : animationTakes)
				{
					Animation animation;
					FetchAnimation(fbxMesh, animationTake, animation);
					mesh.animations.push_back(animation);
				}
			}

			std::function<void(FbxNode*, HierarchyNode&)> TraverseHierarchy = [&TraverseHierarchy](FbxNode* fbxNode, HierarchyNode& node)
			{
				if (fbxNode)
				{
					if (const auto fbxNodeAttribute = fbxNode->GetNodeAttribute())
					{
						node.attribute = fbxNodeAttribute->GetAttributeType();
					}
					else
					{
						node.attribute = FbxNodeAttribute::eUnknown;
					}
					node.name = fbxNode->GetName();

					const int childrenNum = fbxNode->GetChildCount();
					node.children.resize(childrenNum);
					for (int i = 0; i < childrenNum; i++)
					{
						TraverseHierarchy(fbxNode->GetChild(i), node.children.at(i));
					}
				}
			};

			TraverseHierarchy(fbxScene->GetRootNode(), rootNode);

			fbxManager->Destroy();
		}

		void CreateRenderObjects(const bool clearVertexContainer)
		{
			for (auto& mesh : meshes)
			{
				HRESULT hr = S_OK;
				ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();
				{
					D3D11_BUFFER_DESC bufferDesc = {};
					D3D11_SUBRESOURCE_DATA subresourceData = {};

					bufferDesc.ByteWidth = sizeof(T) * mesh.vertices.size();
					bufferDesc.Usage = D3D11_USAGE_DEFAULT;
					bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					bufferDesc.CPUAccessFlags = 0;
					bufferDesc.MiscFlags = 0;
					bufferDesc.StructureByteStride = 0;
					subresourceData.pSysMem = mesh.vertices.data();
					subresourceData.SysMemPitch = 0;
					subresourceData.SysMemSlicePitch = 0;

					hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.ReleaseAndGetAddressOf());
					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
				}
				{
					D3D11_BUFFER_DESC bufferDesc = {};
					D3D11_SUBRESOURCE_DATA subresourceData = {};

					bufferDesc.ByteWidth = sizeof(std::uint32_t) * mesh.indices.size();
					bufferDesc.Usage = D3D11_USAGE_DEFAULT;
					bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
					bufferDesc.CPUAccessFlags = 0;
					bufferDesc.MiscFlags = 0;
					bufferDesc.StructureByteStride = 0;
					subresourceData.pSysMem = mesh.indices.data();
					subresourceData.SysMemPitch = 0;
					subresourceData.SysMemSlicePitch = 0;

					hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.ReleaseAndGetAddressOf());
					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
				}

				if (clearVertexContainer)
				{
					mesh.vertices.clear();
					mesh.indices.clear();
				}
			}
		}

		void CreateShaderResourceViews(const std::string mediaDirectory)
		{
			for (auto iterator = materials.begin(); iterator != materials.end(); ++iterator)
			{
				auto& material = iterator->second;

				for (auto& property : material.properties)
				{
					std::filesystem::path fileName = property.textureFileName.c_str();

					std::string name = fileName.filename().string<char>();

					char concatenatedResourcePath[256];
					strcpy_s(concatenatedResourcePath, mediaDirectory.c_str());
					strcat_s(concatenatedResourcePath, name.c_str());
					if (PathFileExistsA(concatenatedResourcePath) && !PathIsDirectoryA(concatenatedResourcePath))
					{
						LoadTextureFromFile(concatenatedResourcePath, property.SRView.ReleaseAndGetAddressOf(), property.forceSRGB, true);
					}
					else
					{
						std::uint32_t R = static_cast<std::uint8_t>(property.color.x * 255);
						std::uint32_t G = static_cast<std::uint8_t>(property.color.y * 255);
						std::uint32_t B = static_cast<std::uint8_t>(property.color.z * 255);
						std::uint32_t A = static_cast<std::uint8_t>(property.color.w * 255);
						std::uint32_t RGBA = R | (G << 8) | (B << 16) | (A << 24);
						MakeDummyTexture(property.SRView.ReleaseAndGetAddressOf(), RGBA, 16u, property.forceSRGB, true);
					}
				}
			}
		}

		void AddAnimations(const std::string fbxFileName, size_t samplingRate)
		{
			// Create FbxManager
			FbxManager* fbxManager = FbxManager::Create();

			fbxManager->SetIOSettings(FbxIOSettings::Create(fbxManager, IOSROOT));

			FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

			bool importStatus = fbxImporter->Initialize(fbxFileName.c_str(), -1, fbxManager->GetIOSettings());
			_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus(), GetErrorString());

			FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

			importStatus = fbxImporter->Import(fbxScene);
			_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus(), GetErrorString());

			std::vector<AnimationTake> addedAnimationTakes;
			FetchAnimationTakes(fbxScene, addedAnimationTakes, samplingRate);

			std::function<void(FbxNode*, FbxNode*, std::vector<Bone>&, Skeletal&, FbxTime)> Traverse =
				[&Traverse](FbxNode* _node, FbxNode* _parentNode, std::vector<Bone>& _offsetTransforms, Skeletal& _skeletal, FbxTime _time)
			{
				if (const FbxNodeAttribute* fbxNodeAttribute = _node->GetNodeAttribute())
				{
					switch (fbxNodeAttribute->GetAttributeType())
					{
					case FbxNodeAttribute::eMesh:
						break;

					case FbxNodeAttribute::eSkeleton:
						// 骨の空間から地球の空間へのtrnasforms
						FbxAMatrix clusterGlobalCurrentPosition;
						clusterGlobalCurrentPosition = _node->EvaluateGlobalTransform(_time);

						Matrix transform;
						FbxAMatrixToMatrix(clusterGlobalCurrentPosition, transform);

						const std::string name = _node->GetName();
						
						const int boneNum = _offsetTransforms.size();
						for(int boneIndex = 0; boneIndex < boneNum; boneIndex++)
						{
							if (name == _offsetTransforms.at(boneIndex).name)
							{
								_skeletal.bones.at(boneIndex).name = name;
								_skeletal.bones.at(boneIndex).transform = transform;

								// make 'transform_to_parent' that trnasforms from bone space to parent bone space
								_skeletal.bones.at(boneIndex).transformToParent = Matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
								if (_parentNode)
								{
									const FbxAMatrix parentClusterGlobalCurrentPosition = _parentNode->EvaluateGlobalTransform(_time);
									FbxAMatrixToMatrix(parentClusterGlobalCurrentPosition.Inverse() * clusterGlobalCurrentPosition, _skeletal.bones.at(boneIndex).transformToParent);
								}
								break;
							}
						}
						break;

					}
				}
				for (int i = 0; i < _node->GetChildCount(); i++)
				{
					Traverse(_node->GetChild(i), _node, _offsetTransforms, _skeletal, _time);
				}
			};

			for (auto& animationTake : addedAnimationTakes)
			{
				FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animationTake.name.c_str());
				_ASSERT_EXPR_A(animationStack, "This is a problem. The anim stack should be found in the scene!");
				fbxScene->SetCurrentAnimationStack(animationStack);

				for (Mesh<T>& mesh : meshes)
				{
					const int bonesNum = mesh.offsetTransforms.size();

					const float samplingTime = 1.0f / animationTake.samplingRate;

					Animation animation;
					animation.name = animationTake.name;

					FbxTime startTime;
					FbxTime stopTime;
					if (const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animationTake.name.c_str()))
					{
						startTime = takeInfo->mLocalTimeSpan.GetStart();
						stopTime = takeInfo->mLocalTimeSpan.GetStop();
					}
					else
					{
						FbxTimeSpan timeLineTimeSpan;
						fbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeLineTimeSpan);

						startTime = timeLineTimeSpan.GetStart();
						stopTime = timeLineTimeSpan.GetStop();
					}

					FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
					FbxTime samplingStep;
					samplingStep.SetTime(0, 0, 1, 0, 0, timeMode);
					samplingStep = static_cast<FbxLongLong>(samplingStep.Get() * samplingTime);
					for (FbxTime currentTime = startTime; currentTime < stopTime; currentTime += samplingStep)
					{
						Skeletal skeletal;
						skeletal.bones.resize(bonesNum);
						Traverse(fbxScene->GetRootNode(), 0, mesh.offsetTransforms, skeletal, currentTime);
						animation.skeletalTransforms.push_back(skeletal);
					}
					_ASSERT_EXPR_A(animationTake.keyframeSize == animation.skeletalTransforms.size(), "There's no consistency");
					mesh.animations.push_back(animation);
				}
			}
			for (auto& addedAnimationTake : addedAnimationTakes)
			{
				animationTakes.push_back(addedAnimationTake);
			}
			fbxManager->Destroy();
		}

		const Animation* FindAnimation(std::string animationName)
		{
			return meshes.at(0).FindAnimation(animationName);
		}

		std::vector<Animation> FindAnimations()
		{
			return meshes.at(0).animations;
		}

		const Mesh<T>* FindMesh(const std::string* meshName) const
		{
			for (const auto mesh : meshes)
			{
				if (mesh.name == meshName)
				{
					return &mesh;
				}
			}
			return nullptr;
		}

		const Material* FindMaterial(const std::string* materialName) const
		{
			const auto iterator = materials.find(*materialName);
			if (iterator == materials.end())
			{
				return nullptr;
			}
			else
			{
				return &iterator->second;

			}
		}

	public: // variable
		std::vector<AnimationTake> animationTakes;
		HierarchyNode rootNode;
		std::unordered_map<std::string, Material> materials;
		Matrix axisSystemTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		Matrix systemUnitTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		std::string fileName;
		std::vector<Mesh<T>> meshes;

	public:

		void Save()
		{
			std::ofstream stream;
			stream.open(fileName, std::ios::binary);
			cereal::BinaryOutputArchive archive(stream);
			archive(*this);
		}


		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(meshes, animationTakes, materials, rootNode,
				axisSystemTransform,
				systemUnitTransform
			);
		}
	};
}

