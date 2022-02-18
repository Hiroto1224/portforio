#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "MyMath.h"


namespace StartingOver
{
	class Rasterizer
	{
	public:
		Rasterizer(D3D11_FILTER samplerFilter = D3D11_FILTER_ANISOTROPIC,
			D3D11_TEXTURE_ADDRESS_MODE samplerTextureAddressMode = D3D11_TEXTURE_ADDRESS_BORDER,
			Vector4 samplerBoarderColor = Vector4(0, 0, 0, 0));

		//virtual Rasterizer() = default;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader[2];
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	public:
		struct Vertex
		{
			Vector3 position;
			Vector2 texCoord;
			Vector4 color;
		};

		/// <summary>
		/// Transfer to GPU
		/// </summary>
		/// <param name="shaderResourceView"> pID3D11ShaderResourceView </param>
		/// <param name="position"> screen space position x,y </param>
		/// <param name="size"> screen space size x,y </param>
		/// <param name="texturePosition"> texture position x,y </param>
		/// <param name="textureSize"> texture size x,y </param>
		/// <param name="angle"> degree  </param>
		/// <param name="color"> color x,y,z,a </param>
		/// <param name="useVertexShader"> use vertexShader </param>
		/// <param name="usePixelShader"> use pixelShader </param>
		/// <param name="useRasterizerState"> use rasterizerState </param>
		/// <param name="useDepthStencilState"> use DepthStencilState </param>
		/// <param name="useSamplerState"> use Sampler State </param>
		void Blit
		(
			ID3D11ShaderResourceView* shaderResourceView,
			Vector2 position, Vector2 size,
			Vector2 texturePosition, Vector2 textureSize,
			float angle,
			Vector4 color,
			bool useVertexShader = true,
			bool usePixelShader = true,
			bool useRasterizerState = true,
			bool useDepthStencilState = true,
			bool useSamplerState = true
		) const;

		/// <summary>
		/// Transfer to GPU
		/// </summary>
		/// <param name="shaderResourceView"> pID3D11ShaderResourceView </param>
		/// <param name="position"> screen space position x,y </param>
		/// <param name="size"> screen space size x,y </param>
		/// <param name="angle"> degree  </param>
		/// <param name="color"> color x,y,z,a </param>
		/// <param name="useVertexShader"> use vertexShader </param>
		/// <param name="usePixelShader"> use pixelShader </param>
		/// <param name="useRasterizerState"> use rasterizerState </param>
		/// <param name="useDepthStencilState"> use DepthStencilState </param>
		/// <param name="useSamplerState"> use Sampler State </param>
		void Blit
		(
			ID3D11ShaderResourceView* shaderResourceView,
			Vector2 position, Vector2 size,
			float angle = 0.0f,
			Vector4 color = Vector4(1, 1, 1, 1),
			bool useVertexShader = true,
			bool usePixelShader = true,
			bool useRasterizerState = true,
			bool useDepthStencilState = true,
			bool useSamplerState = true

		) const;
	};
}
