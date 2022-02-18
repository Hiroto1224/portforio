#include "../Header/Rasterizer.h"

#include "../Header/Texture.h"
#include "../Header/Shader.h"


namespace StartingOver
{
	Rasterizer::Rasterizer(D3D11_FILTER samplerFilter, D3D11_TEXTURE_ADDRESS_MODE samplerTextureAddressMode, Vector4 samplerBoarderColor)
	{
		HRESULT hr = S_OK;
		ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();


		Vertex vertices[] =
		{
			{Vector3(0,0,0),Vector2(0,0),Vector4(1,1,1,1) },
			{Vector3(0,0,0),Vector2(0,0),Vector4(1,1,1,1) },
			{Vector3(0,0,0),Vector2(0,0),Vector4(1,1,1,1) },
			{Vector3(0,0,0),Vector2(0,0),Vector4(1,1,1,1) },

		};

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(vertices);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = vertices;
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		static const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{"TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"COLOR",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		std::string fileName = "./Shader/Rasterizer_vs.cso";

		CreateVSFromCSO(fileName, vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(),
			const_cast<D3D11_INPUT_ELEMENT_DESC*>(inputElementDesc), _countof(inputElementDesc));

		fileName = "./Shader/Rasterizer_ps.cso";
		CreatePSFromCSO(fileName, pixelShader[0].ReleaseAndGetAddressOf());

		fileName = "./Shader/Rasterizer_ps_ms.cso";

		CreatePSFromCSO(fileName, pixelShader[1].ReleaseAndGetAddressOf());

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = samplerFilter;
		samplerDesc.AddressU = samplerTextureAddressMode;
		samplerDesc.AddressV = samplerTextureAddressMode;
		samplerDesc.AddressW = samplerTextureAddressMode;
		samplerDesc.MipLODBias = 0;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		memcpy(samplerDesc.BorderColor, &samplerBoarderColor, sizeof(Vector4));
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&samplerDesc, samplerState.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0;
		rasterizerDesc.DepthClipEnable = FALSE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	void Rasterizer::Blit(ID3D11ShaderResourceView* shaderResourceView, 
		Vector2 position, Vector2 size, Vector2 texturePosition, 
		Vector2 textureSize, float angle, Vector4 color,
		bool useVertexShader, bool usePixelShader,
		bool useRasterizerState, bool useDepthStencilState, 
		bool useSamplerState) const
	{
		HRESULT hr = S_OK;

		ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceView->GetDesc(&shaderResourceViewDesc);
		bool multiSampled = shaderResourceViewDesc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2DMS;

		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		shaderResourceView->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
		hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2D.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_TEXTURE2D_DESC texture2DDesc;
		texture2D->GetDesc(&texture2DDesc);

		D3D11_VIEWPORT viewport;
		UINT viewportsNum = 1;
		context->RSGetViewports(&viewportsNum, &viewport);
		float screenWidth = viewport.Width;
		float screenHeight = viewport.Height;

		// left top
		float x0 = position.x;
		float y0 = position.y;
		// right top
		float x1 = position.x + size.x;
		float y1 = position.y;
		// left bottom
		float x2 = position.x;
		float y2 = position.y + size.y;
		// right bottom
		float x3 = position.x + size.x;
		float y3 = position.y + size.y;

		// Translate sprite center to origin
		float mx = position.x + size.x * 0.5f;
		float my = position.y + size.y * 0.5f;
		x0 -= mx;
		y0 -= my;
		x1 -= mx;
		y1 -= my;
		x2 -= mx;
		y2 -= my;
		x3 -= mx;
		y3 -= my;

		// Rotate each sprite vertices by angle
		float rx, ry;
		constexpr float radian = 0.01745f;
		float cos = cosf(angle * radian);
		float sin = sinf(angle * radian);
		rx = x0;
		ry = y0;
		x0 = cos * rx + -sin * ry;
		y0 = sin * rx + cos * ry;
		rx = x1;
		ry = y1;
		x1 = cos * rx + -sin * ry;
		y1 = sin * rx + cos * ry;
		rx = x2;
		ry = y2;
		x2 = cos * rx + -sin * ry;
		y2 = sin * rx + cos * ry;
		rx = x3;
		ry = y3;
		x3 = cos * rx + -sin * ry;
		y3 = sin * rx + cos * ry;

		// Translate sprite center to origin position
		x0 += mx;
		y0 += my;
		x1 += mx;
		y1 += my;
		x2 += mx;
		y2 += my;
		x3 += mx;
		y3 += my;

		// Convert to NDC
		x0 = 2.0f * x0 / screenWidth - 1.0f;
		y0 = 1.0f - 2.0f * y0 / screenHeight;
		x1 = 2.0f * x1 / screenWidth - 1.0f;
		y1 = 1.0f - 2.0f * y1 / screenHeight;
		x2 = 2.0f * x2 / screenWidth - 1.0f;
		y2 = 1.0f - 2.0f * y2 / screenHeight;
		x3 = 2.0f * x3 / screenWidth - 1.0f;
		y3 = 1.0f - 2.0f * y3 / screenHeight;


		D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
		D3D11_MAPPED_SUBRESOURCE mappedBuffer;
		hr = context->Map(vertexBuffer.Get(), 0, map, 0, &mappedBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		Vertex* vertices = static_cast<Vertex*>(mappedBuffer.pData);
		vertices[0].position.x = x0;
		vertices[0].position.y = y0;
		vertices[1].position.x = x1;
		vertices[1].position.y = y1;
		vertices[2].position.x = x2;
		vertices[2].position.y = y2;
		vertices[3].position.x = x3;
		vertices[3].position.y = y3;
		vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;

		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

		vertices[0].texCoord.x = (position.x) / texture2DDesc.Width;
		vertices[0].texCoord.y = (position.y) / texture2DDesc.Height;
		vertices[1].texCoord.x = (position.x + size.x) / texture2DDesc.Width;
		vertices[1].texCoord.y = (position.y) / texture2DDesc.Height;
		vertices[2].texCoord.x = (position.x) / texture2DDesc.Width;
		vertices[2].texCoord.y = (position.y + size.y) / texture2DDesc.Height;
		vertices[3].texCoord.x = (position.x + size.x) / texture2DDesc.Width;
		vertices[3].texCoord.y = (position.y + size.y) / texture2DDesc.Height;

		context->Unmap(vertexBuffer.Get(), 0);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		context->IASetInputLayout(inputLayout.Get());

		if (useVertexShader)
		{
			context->VSSetShader(vertexShader.Get(), 0, 0);
		}
		if (usePixelShader)
		{
			if(multiSampled)
			{
				context->PSSetShader(pixelShader[1].Get(), nullptr, 0);
			}else
			{
				context->PSSetShader(pixelShader[0].Get(), nullptr, 0);
			}
		}
		context->PSSetShaderResources(0, 1, &shaderResourceView);

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> cached_rasterizer_state;
		if (useRasterizerState)
		{
			context->RSGetState(cached_rasterizer_state.ReleaseAndGetAddressOf());
			context->RSSetState(rasterizerState.Get());
		}
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> cached_depth_stencil_state;
		if (useDepthStencilState)
		{
			context->OMGetDepthStencilState(cached_depth_stencil_state.ReleaseAndGetAddressOf(), 0);
			context->OMSetDepthStencilState(depthStencilState.Get(), 1);
		}
		Microsoft::WRL::ComPtr<ID3D11SamplerState> cached_sampler_state;
		if (useSamplerState)
		{
			context->PSGetSamplers(0, 1, cached_sampler_state.ReleaseAndGetAddressOf());
			context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
		}
		context->Draw(4, 0);

		context->IASetInputLayout(nullptr);
		context->VSSetShader(nullptr, nullptr, 0);
		context->PSSetShader(nullptr, nullptr, 0);

		ID3D11ShaderResourceView* null_shader_resource_view = nullptr;
		context->PSSetShaderResources(0, 1, &null_shader_resource_view);

		if (cached_rasterizer_state)
		{
			context->RSSetState(cached_rasterizer_state.Get());
		}
		if (cached_depth_stencil_state)
		{
			context->OMSetDepthStencilState(cached_depth_stencil_state.Get(), 1);
		}
		if (cached_sampler_state)
		{
			context->PSSetSamplers(0, 1, cached_sampler_state.GetAddressOf());
		}



	}



	void Rasterizer::Blit(ID3D11ShaderResourceView* shaderResourceView,
	                      const Vector2 position, const Vector2 size, const float angle, const Vector4 color,
	                      const bool useVertexShader, const bool usePixelShader,
	                      const bool useRasterizerState, const bool useDepthStencilState,
	                      const bool useSamplerState) const
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		shaderResourceView->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
		HRESULT hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2D.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_TEXTURE2D_DESC texture2DDesc;
		texture2D->GetDesc(&texture2DDesc);

		Vector2 textureSize;
		textureSize.x = static_cast<float>(texture2DDesc.Width);
		textureSize.y = static_cast<float>(texture2DDesc.Height);

		Blit(shaderResourceView, position, size, Vector2(0, 0), textureSize, angle,
			color, useVertexShader, usePixelShader, useRasterizerState, useDepthStencilState, useSamplerState);

	}



}
