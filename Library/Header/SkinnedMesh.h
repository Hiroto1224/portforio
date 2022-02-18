#pragma once

#include <string>

#include "MyMath.h"
#include "Substance.h"

#include "RenderObject.h"


class SkinnedMesh : public RenderObject
{
public:

	void Initialize(const std::shared_ptr<ExistObject> object) override;

	// debug用関数　
	void DebugUI() override;

	/// <summary>
	/// 描画用関数
	/// </summary>
	void Render() override;

	/// <summary>
	///シャドウ描画用関数　現在未使用 二回描画は重たいから要検討
	/// </summary>
	void RenderShadow() override;

	void Animate() const;

	void AddAnimations(const std::string& fbxFileName, const int samplingRate) const
	{
		substance->AddAnimations(fbxFileName, samplingRate);
	}

private:
	
	//モデルのロード
	void Load() override;
public:
	struct Vertex
	{
		Vector3 position;
		Vector3 normal;
		Vector4 tangent;
		Vector2 texCoord;
		float boneWeights[MAXBONEINFLUENCE];
		std::uint32_t boneIndices[MAXBONEINFLUENCE];

		Vertex(const StartingOver::Vertex& _vertex)
		{
			position = _vertex.position;
			normal = _vertex.normal;
			tangent = _vertex.tangent;
			texCoord = _vertex.texCoord;
			memcpy(boneWeights, _vertex.boneWeight, sizeof(boneWeights));
			memcpy(boneIndices, _vertex.boneIndices, sizeof(boneIndices));
		}
		Vertex() = default;

		template<class T>
		void serialize(T& archive)
		{
			archive
			(
				position,
				normal,
				tangent,
				texCoord,
				boneWeights,
				boneIndices
			);
		}

		static D3D11_INPUT_ELEMENT_DESC* AcquireInputElementDesc(size_t& elementNum)
		{
			static D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "WEIGHTS",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
				{ "BONES",0,DXGI_FORMAT_R32G32B32A32_UINT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			};

			elementNum = _countof(inputElementDesc);
			return inputElementDesc;
		}
	};

	std::unique_ptr<StartingOver::Substance<Vertex>> substance;

	struct ShaderConstants
	{
		Matrix world;
		Matrix boneTranslation[MAXBONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
		Vector4 color;
	};

	std::unique_ptr<StartingOver::ConstantBuffer<ShaderConstants>> constantBuffer;

private:
	std::unique_ptr<StartingOver::VertexShader<Vertex>> vertexShader;
	std::unique_ptr<StartingOver::PixelShader> pixelShader;
	std::unique_ptr<StartingOver::VertexShader<Vertex>> shadowVertexShader;

	
	struct AnimationData
	{
		int animationClip = 0;
		int animationLastClip = 0;
		int animationFrame = 0;
		float animationTick = 0;
		float animationTimer = 1;
	};

	std::unique_ptr<AnimationData> animationData;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<RenderObject>(this));
	}
};

CEREAL_REGISTER_TYPE(SkinnedMesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(RenderObject, SkinnedMesh)