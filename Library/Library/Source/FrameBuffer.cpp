#include "../Header/FrameBuffer.h"
#include "../Header/misc.h"

namespace StartingOver
{
	FrameBuffer::FrameBuffer(float width, float height, bool enableMsaa , 
		int subSamples, DXGI_FORMAT renderTargetTexture2DFormat,
		DXGI_FORMAT depthStencilTexture2DfFormat,bool needRenderTargetShaderResourceView, 
		bool needDepthStencilShaderResourceView, bool generateMips)
	{
		HRESULT hr = S_OK;
		ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();

		std::uint32_t msaaQualityLevel;
		std::uint32_t sampleCount = subSamples;
		hr = device->CheckMultisampleQualityLevels(renderTargetTexture2DFormat, sampleCount, &msaaQualityLevel);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if(renderTargetTexture2DFormat != DXGI_FORMAT_UNKNOWN)
		{
			D3D11_TEXTURE2D_DESC texture2DDesc = {};
			texture2DDesc.Width = width;
			texture2DDesc.Height = height;
			texture2DDesc.ArraySize = 1;
			texture2DDesc.Format = renderTargetTexture2DFormat;
			if(enableMsaa)
			{
				texture2DDesc.SampleDesc.Count = sampleCount;
				texture2DDesc.SampleDesc.Quality = msaaQualityLevel - 1;
			}else
			{
				texture2DDesc.SampleDesc.Count = 1;
				texture2DDesc.SampleDesc.Quality = 0;
			}
			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			if(needRenderTargetShaderResourceView)
			{
				texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			}
			else
			{
				texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
			}
			texture2DDesc.CPUAccessFlags = 0;
			if(generateMips)
			{
				texture2DDesc.MipLevels = 0;
				texture2DDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			}else
			{
				texture2DDesc.MipLevels = 1;
				texture2DDesc.MiscFlags = 0;
			}

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
			hr = device->CreateTexture2D(&texture2DDesc, 0, texture2D.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
			renderTargetViewDesc.Format = texture2DDesc.Format;
			if (enableMsaa)
			{
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
			}else
			{
				renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			}
			hr = device->CreateRenderTargetView(texture2D.Get(), &renderTargetViewDesc, RTView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			if(needRenderTargetShaderResourceView)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
				shaderResourceViewDesc.Format = texture2DDesc.Format;
				if(enableMsaa)
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
				}else
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				}
				if(generateMips)
				{
					shaderResourceViewDesc.Texture2D.MipLevels = -1;
				}else
				{
					shaderResourceViewDesc.Texture2D.MipLevels = 1;
				}
				hr = device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, renderTargetShaderResourceView.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
		}
		if(depthStencilTexture2DfFormat != DXGI_FORMAT_UNKNOWN)
		{
			constexpr DXGI_FORMAT depthStencilFormats[3][3] =
			{
				{ DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_R24_UNORM_X8_TYPELESS },
				{ DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_R32_FLOAT },
				{ DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_D16_UNORM, DXGI_FORMAT_R16_UNORM },
			};
			int depthStencilTexture2DFormatIndex = 0;
			switch (depthStencilTexture2DfFormat)
			{
			case DXGI_FORMAT_R24G8_TYPELESS:
				depthStencilTexture2DFormatIndex = 0;
				break;
			case DXGI_FORMAT_R32_TYPELESS:
				depthStencilTexture2DFormatIndex = 1;
				break;
			case DXGI_FORMAT_R16_TYPELESS:
				depthStencilTexture2DFormatIndex = 2;
				break;
			default: ;
			}
			D3D11_TEXTURE2D_DESC texture2DDesc = {};
			texture2DDesc.Width = width;
			texture2DDesc.Height = height;
			texture2DDesc.MipLevels = 1;
			texture2DDesc.ArraySize = 1;
			texture2DDesc.Format = depthStencilFormats[depthStencilTexture2DFormatIndex][0];
			if(enableMsaa)
			{
				texture2DDesc.SampleDesc.Count = sampleCount;
				texture2DDesc.SampleDesc.Quality = msaaQualityLevel - 1;
			}else
			{
				texture2DDesc.SampleDesc.Count = 1;
				texture2DDesc.SampleDesc.Quality = 0;
			}
			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			if(needDepthStencilShaderResourceView)
			{
				texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			}
			else
			{
				texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			}
			texture2DDesc.CPUAccessFlags = 0;
			texture2DDesc.MiscFlags = 0;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
			hr = device->CreateTexture2D(&texture2DDesc, 0, texture2D.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
			depthStencilViewDesc.Format = depthStencilFormats[depthStencilTexture2DFormatIndex][1];
			if(enableMsaa)
			{
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			}else
			{
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			}
			depthStencilViewDesc.Flags = 0;
			hr = device->CreateDepthStencilView(texture2D.Get(), &depthStencilViewDesc, DSView.GetAddressOf());

			if(needDepthStencilShaderResourceView)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
				shaderResourceViewDesc.Format = depthStencilFormats[depthStencilTexture2DFormatIndex][2];
				if(enableMsaa)
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
				}else
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				}
				shaderResourceViewDesc.Texture2D.MipLevels = 1;
				hr = device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, depthStencilShaderResourceView.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
		}
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
	}

	FrameBuffer::FrameBuffer(ID3D11RenderTargetView* renderTargetView, DXGI_FORMAT depthStencilTexture2DFormat, bool _needDepthStencilShaderResourceView)
	{
		HRESULT hr = S_OK;
		ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
		renderTargetView->GetDesc(&renderTargetViewDesc);
		bool enableMsaa = renderTargetViewDesc.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DMS;

		D3D11_TEXTURE2D_DESC shareTexture2DDesc;
		RTView = renderTargetView;
		{
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			renderTargetView->GetResource(resource.GetAddressOf());

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
			hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2D.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			texture2D->GetDesc(&shareTexture2DDesc);
			if(shareTexture2DDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
				shaderResourceViewDesc.Format = shareTexture2DDesc.Format;
				if(enableMsaa)
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
				}else
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				}
				shaderResourceViewDesc.Texture2D.MipLevels = 1;
				hr = device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, renderTargetShaderResourceView.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
		}
		{
			const DXGI_FORMAT depthStencilFormats[3][3] =
			{
				{ DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_R24_UNORM_X8_TYPELESS },
				{ DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_R32_FLOAT },
				{ DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_D16_UNORM, DXGI_FORMAT_R16_UNORM },
			};
			int depthStencilTexture2DFormatIndex = 0;
			switch (depthStencilTexture2DFormat)
			{
			case DXGI_FORMAT_R24G8_TYPELESS:
				depthStencilTexture2DFormatIndex = 0;
				break;
			case DXGI_FORMAT_R32_TYPELESS:
				depthStencilTexture2DFormatIndex = 1;
				break;
			case DXGI_FORMAT_R16_TYPELESS:
				depthStencilTexture2DFormatIndex = 2;
				break;
			default: ;
			}
			D3D11_TEXTURE2D_DESC texture2DDesc = {};
			texture2DDesc.Width = shareTexture2DDesc.Width;
			texture2DDesc.Height = shareTexture2DDesc.Height;
			texture2DDesc.MipLevels = 1;
			texture2DDesc.ArraySize = 1;
			texture2DDesc.Format = depthStencilFormats[depthStencilTexture2DFormatIndex][0];
			texture2DDesc.SampleDesc.Count = shareTexture2DDesc.SampleDesc.Count;
			texture2DDesc.SampleDesc.Quality = shareTexture2DDesc.SampleDesc.Quality;
			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			if (_needDepthStencilShaderResourceView)
			{
				texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			}else
			{
				texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			}
			texture2DDesc.CPUAccessFlags = 0;
			texture2DDesc.MiscFlags = 0;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
			hr = device->CreateTexture2D(&texture2DDesc, 0, texture2D.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
			depthStencilViewDesc.Format = depthStencilFormats[depthStencilTexture2DFormatIndex][1];
			if (enableMsaa)
			{
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			}
			hr = device->CreateDepthStencilView(texture2D.Get(), &depthStencilViewDesc, DSView.GetAddressOf());
			if (_needDepthStencilShaderResourceView)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
				shaderResourceViewDesc.Format = depthStencilFormats[depthStencilTexture2DFormatIndex][2];
				if (enableMsaa)
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				}
				shaderResourceViewDesc.Texture2D.MipLevels = 1;
				hr = device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, depthStencilShaderResourceView.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
		}
		viewport.Width = static_cast<float>(shareTexture2DDesc.Width);
		viewport.Height = static_cast<float>(shareTexture2DDesc.Height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

	}



}
