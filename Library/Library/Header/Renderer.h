#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Data.h"

#include <d3d11.h>
#include <wrl.h>

//#include <cereal/cereal.hpp>
//#include <cereal/access.hpp>
//#include <cereal/types/polymorphic.hpp>
//#include <cereal/archives/json.hpp>

class Renderer
{
public:
	virtual ~Renderer() = default;
	virtual void Initialize(std::shared_ptr<Data> _data) = 0;

	virtual void Render() = 0;

protected:
	void CreateVertexBuffer(const void* _vertices,const u_int _numVertex);
	void CreateIndexBuffer(const u_int* _indices, const u_int _numIndex);

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	//std::unique_ptr<Shader> shader;
	std::unique_ptr<Texture> texture;
	std::shared_ptr<Data> data;
public:
	//friend class cereal::access;
	//template<class Archive>
	//void serialize(Archive& archive)
	//{

	//}

};
