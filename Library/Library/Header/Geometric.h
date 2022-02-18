#pragma once
#include <fbxsdk.h>
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <string>
#include "MyMath.h"
#include "Substance.h"

#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/archives/binary.hpp>


#define MAXBONEINFLUENCES 4
#define MAXBONES 128
namespace StartingOver
{
	namespace Geometric
	{
		struct Vertex
		{
			Vector3 position = {};
			Vector3 normal = {};
			Vector3 tanget = {};
			Vector2 texCoord = {};
			Vector4 color = { 1,1,1,1 };

			float boneWeights[MAXBONEINFLUENCES] = { 1,0,0,0 };
			unsigned int boneIndices[MAXBONEINFLUENCES] = {};

			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive
				(
					position,
					normal,
					tanget,
					texCoord,
					color,
					boneWeights,
					boneIndices
				);
			}
		};

		struct Bone
		{
			std::string name;

			Matrix transform; // from bone space to mesh space
			Matrix offsetTransform; // from mesh space to bone spce

			Matrix parentTransform; // from bone space to parent bone space

			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive
				(
					name,
					transform,
					offsetTransform,
					parentTransform
				);
			}
		};


		struct Skeletal
		{
			std::vector<Bone> bones;
			const Bone* Find(const std::string& boneName) const
			{
				for (const Bone& bone : bones)
				{
					if (bone.name == boneName)
					{
						return &bone;
					}
				}
			}

			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(bones);
			}
		};

		struct Animation
		{
			std::string name;
			std::vector<Skeletal> skeletals;

			float samplingTime;

			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(name, skeletals, samplingTime);
			}
		};

		/// <summary>
		/// 
		/// </summary>
		/// <param name="fbxMesh"></param>
		/// <param name="animations"></param>
		/// <param name="samplingRate"> フレーム/秒 </param>
		void FetchAnimations(FbxMesh* fbxMesh, std::vector<Animation>& animations, int samplingRate);

		struct AnimationTake
		{
			std::string name;
			size_t samplingRate;
			size_t keyframeCount = 0;

			friend class cereal::access;
			template<class T>
			void serialize(T& archive)
			{
				archive(name, samplingRate, keyframeCount);
			}

		};

		/// <summary>
		/// 
		/// </summary>
		/// <param name="scene"></param>
		/// <param name="animationTakes"></param>
		/// <param name="samplingRate">フレーム/秒 </param>
		void FetchAnimationTakes(FbxScene* scene, std::vector<Geometric::AnimationTake>& animationTakes, int samplingRate);

		struct Material
		{
			std::string name;
			unsigned int indexStart = 0; // index bufferの開始番号
			unsigned int indexCount = 0; // verticesとindicesのサイズ

			struct Property
			{
				Vector4 color = { 0.8f, 0.8f, 0.8f, 1.0f }; // w channel is used as shininess by only specular.
				float factor = 1.0f;

				std::string textureFileName;
				float uScale = 1.0f;
				float vScale = 1.0f;

				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRView;

				friend class cereal::access;
				template<class T>
				void serialize(T& archive)
				{
					archive
					(
						factor,
						color,
						textureFileName,
						uScale, vScale
					);
				}
			};

			Property ambient;
			Property diffuse;
			Property specular;
			Property bump;
			Property normalMap;

			friend class cereal::access;
			template<class T>
			void serialize(T& archive)
			{
				archive(name, indexCount, indexStart, ambient, diffuse, specular, bump, normalMap);
			}
		};

		struct BoundingBox
		{
			Vector3 min{ +FLT_MAX,+FLT_MAX,+FLT_MAX };
			Vector3 max{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

			friend class cereal::access;
			template<class T>
			void serialize(T& archive)
			{
				archive(min,max);
			}

		};


		void FetchMaterials(FbxMesh* _fbxMesh, std::vector<Material>& _materials);
		void FetchVerticesAndIndices(FbxMesh* _fbxMesh, std::vector<Vertex>& _vertices, std::vector<unsigned int>& _indices, std::vector<Material>& _materials);


		template <class T>
		struct Substance
		{
			struct Mesh
			{
				std::string name;

				std::vector<T> vertices;
				std::vector<unsigned int> indices;

				Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
				Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

				std::vector<Material> materials;
				const std::string FindMaterial(int _indexOfTriangle) const
				{
					size_t indexOfIndices = 3 * _indexOfTriangle;
					for (const Material& material : materials)
					{
						if (material.indexStart <= indexOfIndices &&
							material.indexStart + material.indexCount > indexOfIndices)
						{
							if (material.name.empty())
							{
								return "";
							}
							else
							{
								return material.name.c_str();
							}
						}
					}

					return 0;
				}

				std::vector<Animation> animations;

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
						name, vertices, indices, materials,
						globalTransform,
						animations, boundingBox
					);
				}

			};


			const Mesh* FindMesh(const std::string* meshName) const
			{
				for (const Mesh& mesh : meshes)
				{
					if (mesh.name == meshName)
					{
						return &mesh;
					}
				}
				return 0;
			}

			Substance() = default;
			virtual ~Substance() = default;
			Substance(Substance&) = delete;
			Substance& operator=(Substance&) = delete;
			Substance(const std::string _fbxFileName, bool _triangulate, int _animationSamplingRate = 0, const std::string _extension = ".bin")
			{
				if (PathFileExistsA((std::string(_fbxFileName) + _extension).c_str()))
				{
					// 読み込み
					load((std::string(_fbxFileName) + _extension).c_str());

				}
				else
				{
					// fbxファイルをシリアライズする
					CreateFromFbx(_fbxFileName, _triangulate, _animationSamplingRate);

					save((std::string(_fbxFileName) + _extension).c_str());

				}
			}

			Substance(const std::string _fbxFileName, bool _triangulate, const std::string _animationFileNames[], size_t _animationNumSize, int _animationSamplingRate = 0, const std::string _extension = ".bin")
			{

				if (PathFileExistsA((std::string(_fbxFileName) + _extension).c_str()))
				{
					load((std::string(_fbxFileName) + _extension).c_str());

				}
				else
				{
					CreateFromFbx(_fbxFileName, _triangulate, _animationSamplingRate);
				

					save((std::string(_fbxFileName) + _extension).c_str());

				}
			}

			void save(std::string name)
			{
				std::ofstream stream;
				stream.open(name, std::ios::binary);
				cereal::BinaryOutputArchive archive(stream);
				archive(meshes, animationTakes, hierarchy);
			}

			void load(std::string name)
			{
				std::ifstream stream;
				stream.open(name, std::ios::binary);
				cereal::BinaryInputArchive archive(stream);
				archive(meshes, animationTakes, hierarchy);
			}

			void CreateFromFbx(const std::string _fbxFileName, bool _triangulate, int _animationSamplingRate)
			{
				// Create FbxManager
				FbxManager* fbxManager = FbxManager::Create();

				fbxManager->SetIOSettings(FbxIOSettings::Create(fbxManager, IOSROOT));

				FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

				bool importStatus = fbxImporter->Initialize(_fbxFileName.c_str(), -1, fbxManager->GetIOSettings());
				_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus(), GetErrorString());

				FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

				importStatus = fbxImporter->Import(fbxScene);
				_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus(), GetErrorString());


				// 左手座標系に変換 (Y-UP Z-FRONT)
				FbxAxisSystem axisSystemReference = fbxScene->GetGlobalSettings().GetAxisSystem();
				FbxAxisSystem::ECoordSystem coordSystem = axisSystemReference.GetCoorSystem();
				_ASSERT_EXPR(coordSystem == FbxAxisSystem::eRightHanded, L"Not support X-Up axis system");

				int upVectorSign = 1;
				FbxAxisSystem::EUpVector upVector = axisSystemReference.GetUpVector(upVectorSign);
				_ASSERT_EXPR(upVector == FbxAxisSystem::eYAxis || upVector == FbxAxisSystem::eZAxis, L"Not support X-Up axis system");

				int frontVectorSign = 1;
				FbxAxisSystem::EFrontVector frontVector = axisSystemReference.GetFrontVector(frontVectorSign);
				_ASSERT_EXPR(frontVectorSign == FbxAxisSystem::eParityOdd || frontVectorSign == 1, L"Not support X-Up axis system");

				//FbxAxisSystem::ECoordSystem coordSystem = axisSystemReference.GetCoorSystem();

				Matrix axisSystemConversion = Matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
				axisSystemConversion._11 = -1;

				// Convert unit system 
				Matrix unitSystemConversion = Matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
				if (fbxScene->GetGlobalSettings().GetSystemUnit() == FbxSystemUnit::cm)
				{
					unitSystemConversion._11 = unitSystemConversion._22 = unitSystemConversion._33 = 0.01f;
				}

				// Convert mesh. NURBS and patch into triangle mesh
				if (_triangulate)
				{
					fbxsdk::FbxGeometryConverter geometryConverter(fbxManager);
					geometryConverter.Triangulate(fbxScene, true);
				}

				// Fetch node attributes and materials under this node recursively.
				std::vector<FbxNode*> fetchedMeshes;
				std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
				{
					if (node)
					{
						FbxNodeAttribute* fbxNodeAttribute = node->GetNodeAttribute();
						if (fbxNodeAttribute)
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
							traverse(node->GetChild(i));
						}
					}
				};

				traverse(fbxScene->GetRootNode());

				meshes.resize(fetchedMeshes.size());

				for (size_t i = 0; i < fetchedMeshes.size(); i++)
				{
					FbxMesh* fbxMesh = fetchedMeshes.at(i)->GetMesh();

					Mesh& mesh = meshes.at(i);
					mesh.name = fbxMesh->GetNode()->GetName();

					FbxAMatrix gloabalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
					FbxAMatrixToMatrix(gloabalTransform, mesh.globalTransform);


					// Convert axis system & unit system
					//DirectX::XMStoreFloat4x4(&mesh.gloabalTransform, DirectX::XMLoadFloat4x4(&mesh.gloabalTransform) * DirectX::XMLoadFloat4x4(&unitSystemConversion) * DirectX::XMLoadFloat4x4(&axisSystemConversion));
					Matrix convert = mesh.globalTransform * unitSystemConversion * axisSystemConversion;
					mesh.globalTransform = convert;

					FetchMaterials(fbxMesh, mesh.materials);

					std::vector<Vertex> vertices;
					FetchVerticesAndIndices(fbxMesh, vertices, mesh.indices, mesh.materials);

					for (const Vertex& v : vertices)
					{
						mesh.vertices.push_back(v);
						mesh.boundingBox.max.x = std::max<float>(v.position.x, mesh.boundingBox.max.x);
						mesh.boundingBox.max.y = std::max<float>(v.position.y, mesh.boundingBox.max.y);
						mesh.boundingBox.max.z = std::max<float>(v.position.z, mesh.boundingBox.max.z);
						mesh.boundingBox.min.x = std::min<float>(v.position.x, mesh.boundingBox.min.x);
						mesh.boundingBox.min.y = std::min<float>(v.position.y, mesh.boundingBox.min.y);
						mesh.boundingBox.min.z = std::min<float>(v.position.z, mesh.boundingBox.min.z);
					}

					FetchAnimations(fbxMesh, mesh.animations, _animationSamplingRate);
				}
				FetchAnimationTakes(fbxScene, animationTakes, _animationSamplingRate);

				std::function<void(FbxNode*, HierarchyNode*, HierarchyNode&)> TraverseHierarchy =
					[&TraverseHierarchy](FbxNode* fbxNode, HierarchyNode* parent, HierarchyNode& node)
				{
					if (fbxNode)
					{
						FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();
						if (fbxNodeAttribute)
						{
							node.attribute = fbxNodeAttribute->GetAttributeType();
						}
						else
						{
							node.attribute = FbxNodeAttribute::eUnknown;
						}
						node.name = fbxNode->GetName();

						int childrenNumber = fbxNode->GetChildCount();
						node.children.resize(childrenNumber);
						for (int i = 0; i < childrenNumber; i++)
						{
							TraverseHierarchy(fbxNode->GetChild(i), &node, node.children.at(i));
						}
					}

				};
				TraverseHierarchy(fbxScene->GetRootNode(), 0, hierarchy);
				fbxManager->Destroy();
			}

			void CreateRenderObjects(bool _clearVertexContainer)
			{
				ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();

				for (Mesh& mesh : meshes)
				{
					HRESULT hr = S_OK;
					{
						D3D11_BUFFER_DESC bufferDesc = {};
						D3D11_SUBRESOURCE_DATA subResourceData = {};

						bufferDesc.ByteWidth = sizeof(T) * mesh.vertices.size();
						bufferDesc.Usage = D3D11_USAGE_DEFAULT;
						bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
						bufferDesc.CPUAccessFlags = 0;
						bufferDesc.MiscFlags = 0;
						bufferDesc.StructureByteStride = 0;
						subResourceData.pSysMem = mesh.vertices.data();
						subResourceData.SysMemPitch = 0;
						subResourceData.SysMemSlicePitch = 0;

						hr = device->CreateBuffer(&bufferDesc, &subResourceData, mesh.vertexBuffer.ReleaseAndGetAddressOf());
						_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
					}
					{
						D3D11_BUFFER_DESC bufferDesc = {};
						D3D11_SUBRESOURCE_DATA subResourceData = {};

						bufferDesc.ByteWidth = sizeof(T) * mesh.indices.size();
						bufferDesc.Usage = D3D11_USAGE_DEFAULT;
						bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
						bufferDesc.CPUAccessFlags = 0;
						bufferDesc.MiscFlags = 0;
						bufferDesc.StructureByteStride = 0;
						subResourceData.pSysMem = mesh.indices.data();
						subResourceData.SysMemPitch = 0;
						subResourceData.SysMemSlicePitch = 0;

						hr = device->CreateBuffer(&bufferDesc, &subResourceData, mesh.indexBuffer.ReleaseAndGetAddressOf());
						_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
					}
					if (_clearVertexContainer)
					{
						mesh.vertices.clear();
						mesh.indices.clear();
					}
				}
			}

			void CreateShaderResourceView(std::string mediaDirectory, bool forceSRGB, bool enableCaching)
			{
				std::function<void(const Vector4&, DWORD&)> Convert = [&](const Vector4& color, DWORD& RGBA)
				{
					DWORD R = static_cast<BYTE>(color.x * 255);
					DWORD G = static_cast<BYTE>(color.y * 255);
					DWORD B = static_cast<BYTE>(color.z * 255);
					DWORD A = static_cast<BYTE>(color.w * 255);
					RGBA = R | (G << 8) | (B << 16) | (A << 24);
				};

				for (Mesh mesh : meshes)
				{
					for (Material& material : mesh.materials)
					{
						if (!material.diffuse.textureFileName.empty())
						{
							char combineResourcePath[256];
							strcpy_s(combineResourcePath, mediaDirectory.c_str());
							strcat_s(combineResourcePath, material.diffuse.textureFileName.c_str());
							LoadTextureFromFile(combineResourcePath, material.diffuse.SRView.ReleaseAndGetAddressOf(), forceSRGB, enableCaching);
						}
						else
						{
							DWORD RGBA = 0;
							Convert(material.diffuse.color, RGBA);
							MakeDummyTexture(material.diffuse.SRView.ReleaseAndGetAddressOf(), RGBA, 1, forceSRGB, enableCaching);
						}
						if (!material.ambient.textureFileName.empty())
						{
							char combineResourcePath[256];
							strcpy_s(combineResourcePath, mediaDirectory.c_str());
							strcat_s(combineResourcePath, material.ambient.textureFileName.c_str());
							LoadTextureFromFile(combineResourcePath, material.ambient.SRView.ReleaseAndGetAddressOf(), forceSRGB, enableCaching);

						}
						else
						{
							DWORD RGBA = 0;
							Convert(material.ambient.color, RGBA);
							MakeDummyTexture(material.ambient.SRView.ReleaseAndGetAddressOf(), RGBA, 1, forceSRGB, enableCaching);

						}
						if (!material.specular.textureFileName.empty())
						{
							char combineResourcePath[256];
							strcpy_s(combineResourcePath, mediaDirectory.c_str());
							strcat_s(combineResourcePath, material.specular.textureFileName.c_str());
							LoadTextureFromFile(combineResourcePath, material.specular.SRView.ReleaseAndGetAddressOf(), forceSRGB, enableCaching);
						}
						else
						{
							DWORD RGBA = 0;
							Convert(material.specular.color, RGBA);
							MakeDummyTexture(material.specular.SRView.ReleaseAndGetAddressOf(), RGBA, 1, forceSRGB, enableCaching);
						}
						if (!material.normalMap.textureFileName.empty())
						{
							char combineResourcePath[256];
							strcpy_s(combineResourcePath, mediaDirectory.c_str());
							strcat_s(combineResourcePath, material.normalMap.textureFileName.c_str());
							LoadTextureFromFile(combineResourcePath, material.normalMap.SRView.ReleaseAndGetAddressOf(), false, enableCaching);
						}
						else
						{
							MakeDummyTexture(material.normalMap.SRView.ReleaseAndGetAddressOf(), 0xFFFF7F7F, 1, false, enableCaching);
						}

						if (!material.bump.textureFileName.empty())
						{
							char combineResourcePath[256];
							strcpy_s(combineResourcePath, mediaDirectory.c_str());
							strcat_s(combineResourcePath, material.bump.textureFileName.c_str());
							LoadTextureFromFile(combineResourcePath, material.bump.SRView.ReleaseAndGetAddressOf(), false, enableCaching);
						}
						else
						{
							MakeDummyTexture(material.bump.SRView.ReleaseAndGetAddressOf(), 0xFFFF7F7F, 1, false, enableCaching);
						}
					}
				}

			}
			struct HierarchyNode
			{
				FbxNodeAttribute::EType attribute;
				std::string name;
				std::vector<HierarchyNode> children;

				friend class cereal::access;
				template<class Archive>
				void serialize(Archive& archive)
				{
					archive(attribute, name, children);
				}
			};

			std::vector<Mesh> meshes;
			std::vector<AnimationTake> animationTakes;
			HierarchyNode hierarchy;

			friend class cereal::access;
			template<class Archive>
			void serialize(Archive archive)
			{
				archive(meshes, animationTakes, hierarchy);
			}
		};

		template<class T>
		void CreateCube(std::vector<T>& vertices,std::vector<unsigned int>&indices)
		{
			Vertex vertex[24];

			float w2 = 0.5f;
			float h2 = 0.5f;
			float d2 = 0.5f;

			auto SetVertex = [](Vertex& v, float x, float y, float z, float nx, float ny, float nz, float tx, float ty, float tz, float tu, float tv, float r = 1.0, float g = 1.0f, float b = 1.0f, float a = 1.0f)
			{
				v.position = Vector3(x, y, z);
				v.normal = Vector3(nx, ny, nz);
				v.tanget = Vector3(tx, ty, tz);
				v.texCoord = Vector2(tu, tv);
			};

			SetVertex(vertex[0], -w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
			SetVertex(vertex[1], -w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			SetVertex(vertex[2], +w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
			SetVertex(vertex[3], +w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);


			SetVertex(vertex[4], -w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
			SetVertex(vertex[5], +w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
			SetVertex(vertex[6], +w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			SetVertex(vertex[7], -w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);


			SetVertex(vertex[8], -w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
			SetVertex(vertex[9], -w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			SetVertex(vertex[10], +w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
			SetVertex(vertex[11], +w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

			SetVertex(vertex[12], -w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
			SetVertex(vertex[13], +w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
			SetVertex(vertex[14], +w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			SetVertex(vertex[15], -w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);


			SetVertex(vertex[16], -w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
			SetVertex(vertex[17], -w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
			SetVertex(vertex[18], -w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
			SetVertex(vertex[19], -w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);


			SetVertex(vertex[20], +w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
			SetVertex(vertex[21], +w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
			SetVertex(vertex[22], +w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
			SetVertex(vertex[23], +w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

			vertices.assign(&vertex[0], &vertex[24]);

			unsigned int index[36];

			index[0] = 0; index[1] = 1; index[2] = 2;
			index[3] = 0; index[4] = 2; index[5] = 3;

			index[6] = 4; index[7] = 5; index[8] = 6;
			index[9] = 4; index[10] = 6; index[11] = 7;

			index[12] = 8; index[13] = 9; index[14] = 10;
			index[15] = 8; index[16] = 10; index[17] = 11;

			index[18] = 12; index[19] = 13; index[20] = 14;
			index[21] = 12; index[22] = 14; index[23] = 15;

			index[24] = 16; index[25] = 17; index[26] = 18;
			index[27] = 16; index[28] = 18; index[29] = 19;

			index[30] = 20; index[31] = 21; index[32] = 22;
			index[33] = 20; index[34] = 22; index[35] = 23;

			indices.assign(&index[0], &index[36]);
		}
	}

	inline void FbxMatrixToMatrix(const FbxAMatrix &fbxAMatrix, Matrix& matrix)
	{
		for(int row = 0;row < 4;row++)
		{
			for(int column = 0;column < 4;column++)
			{
				matrix.m[row][column] = static_cast<float>(fbxAMatrix[row][column]);
			}
		}

	}

}