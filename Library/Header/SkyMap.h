#pragma once
#include <string>
#include "MyMath.h"
#include "Geometric.h"
#include "RenderObject.h"

class SkyMap : public RenderObject
{
public:
	SkyMap() {};

	void Initialize(const std::shared_ptr<ExistObject> existObject) override;

	void Render() override;

private:
	struct Vertex
	{
		Vector3 position;

		Vertex(const StartingOver::Geometric::Vertex &vertex)
		{
			position = vertex.position;
		}
		Vertex() = default;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				position.x, position.y, position.z
			);
		}

		static D3D11_INPUT_ELEMENT_DESC* AcquireInputElementDesc(size_t& elementNum)
		{
			static D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
			{
				{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			};
			elementNum = _countof(inputElementDesc);
			return inputElementDesc;
		}

	
	};

	std::unique_ptr<StartingOver::Geometric::Substance<Vertex>> substance;

	std::unique_ptr<StartingOver::VertexShader<Vertex>> vertexShader;
	std::unique_ptr<StartingOver::PixelShader> pixelShader;
	std::unique_ptr<StartingOver::VertexShader<Vertex>> shadowVertexShader;

	struct ShaderConstants
	{
		Matrix world;
		Vector4 color;
	};

	std::unique_ptr<StartingOver::ConstantBuffer<ShaderConstants>> constantBuffer;

public:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;



};
