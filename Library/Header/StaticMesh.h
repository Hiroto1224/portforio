#pragma once
#include <d3d11.h>
#include <memory>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>

#include "RenderObject.h"
#include "Texture.h"
#include "MyMath.h"
#include "ConstantBuffer.h"
#include "Substance.h"

class StaticMesh : public RenderObject
{
public:
	StaticMesh() { };

	~StaticMesh() override = default;
	

	void Initialize(const std::shared_ptr<ExistObject> object) override;

	void DebugUI() override;

	void Render() override;

	void RenderShadow() override;

	struct ShaderConstants
	{
		Matrix world;
		Vector4 color = Vector4(1, 1, 1, 1);
	};

	std::unique_ptr<StartingOver::ConstantBuffer<ShaderConstants>> constantBuffer;

	struct Vertex
	{
		Vector3 position;
		Vector3 normal;
		Vector4 tangent;
		Vector2 texCoord;

		Vertex(const StartingOver::Vertex& _vertex)
		{
			position = _vertex.position;
			normal = _vertex.normal;
			tangent = _vertex.tangent;
			texCoord = _vertex.texCoord;
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
				texCoord
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
			};
			elementNum = _countof(inputElementDesc);
			return inputElementDesc;
		}

	};

	std::unique_ptr<StartingOver::Substance<Vertex>> substance;

protected:
	void Load() override;

private:

	std::unique_ptr<StartingOver::VertexShader<Vertex>> vertexShader;
	std::unique_ptr<StartingOver::PixelShader> pixelShader;
	std::unique_ptr<StartingOver::VertexShader<Vertex>> shadowVertexShader;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<RenderObject>(this));
	}
};


CEREAL_REGISTER_TYPE(StaticMesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(RenderObject, StaticMesh)