#include "../Header/DirectXSystem.h"
#include <DirectXTex/DirectXTex.h>
#include <Shlwapi.h>
#include <mutex>
#include <unordered_map>
#include <clocale>

#include "../Header/Texture.h"
#include "../Header/misc.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <map>
#define TINYEXR_IMPLEMENTATION
#undef max
#undef min
#include <tinyexr.h>
#include <sstream>

namespace StartingOver
{
	static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cachedTextures;

	void LoadTextureFromFile(const std::string& textureName, ID3D11ShaderResourceView** _shaderResourceView, bool _forceSRGB, bool _enableCaching)
	{

		wchar_t fileName[256];
		MultiByteToWideChar(CP_ACP, 0, textureName.c_str(), static_cast<int>(strlen(textureName.c_str()) + 1), fileName, 256);

		HRESULT hr = S_OK;

		if(const auto iterator = cachedTextures.find(fileName); iterator != cachedTextures.end())
		{
			*_shaderResourceView = iterator->second.Get();
			(*_shaderResourceView)->AddRef();
		}
		else
		{
			DirectX::TexMetadata metadata;
			DirectX::ScratchImage image;

			std::wstring extension = PathFindExtensionW(fileName);
			std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);

			if (L".png" == extension || L".jpeg" == extension || L".jpg" == extension || L".jpe" == extension || L".gif" == extension || L".tiff" == extension || L".tif" == extension || L".bmp" == extension)
			{
				hr = DirectX::LoadFromWICFile(fileName, DirectX::WIC_FLAGS_NONE, &metadata, image);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			else if (L".dds" == extension)
			{
				hr = DirectX::LoadFromDDSFile(fileName, DirectX::DDS_FLAGS_NONE, &metadata, image);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			else if (L".tga" == extension || L".vda" == extension || L".icb" == extension || L".vst" == extension)
			{
				hr = DirectX::LoadFromTGAFile(fileName, &metadata, image);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			else if (L".hdr" == extension)
			{
				hr = DirectX::LoadFromHDRFile(fileName, &metadata, image);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			else
			{
				_ASSERT_EXPR(false, L"File format not supported.");
			}

			ID3D11Device* device = DirectX11Manager::GetDevice();

			hr = DirectX::CreateShaderResourceViewEx(
				device,
				image.GetImages(),
				image.GetImageCount(),
				metadata,
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_SHADER_RESOURCE,
				0,
				D3D11_RESOURCE_MISC_TEXTURECUBE,
				_forceSRGB,
				_shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			if(_enableCaching)
			{
				cachedTextures.insert(std::make_pair(fileName, *_shaderResourceView));
			}
		}

	}

	void LoadHdrTextureFromFile(const std::string& hdrTextureName, ID3D11ShaderResourceView** shaderResourceView, bool enableCaching)
	{
		wchar_t fileName[256];
		MultiByteToWideChar(CP_ACP, 0, hdrTextureName.c_str(), static_cast<int>(strlen(hdrTextureName.c_str()) + 1), fileName, 256);
		std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>::iterator it = cachedTextures.find(fileName);

		if(it != cachedTextures.end())
		{
			*shaderResourceView = it->second.Get();
			(*shaderResourceView)->AddRef();
		}
		else
		{
			int width, height;
			float* outRGBA;

			const char* err = nullptr;
			int ret = LoadEXR(&outRGBA, &width, &height, hdrTextureName.c_str(), &err);

			HRESULT hr = S_OK;

			ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();

			D3D11_TEXTURE2D_DESC texture2DDesc = {};
			texture2DDesc.Width = width;
			texture2DDesc.Height = height;
			texture2DDesc.MipLevels = 1;
			texture2DDesc.ArraySize = 1;
			texture2DDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			texture2DDesc.SampleDesc.Count = 1;
			texture2DDesc.SampleDesc.Quality = 0;
			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texture2DDesc.CPUAccessFlags = 0;
			texture2DDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pSysMem = outRGBA;
			subresourceData.SysMemPitch = sizeof(float) * 4 * width;
			subresourceData.SysMemSlicePitch = 0;

			ID3D11Texture2D* texture2D;
			hr = device->CreateTexture2D(&texture2DDesc, &subresourceData, &texture2D);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
			shaderResourceViewDesc.Format = texture2DDesc.Format;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;

			hr = device->CreateShaderResourceView(texture2D, &shaderResourceViewDesc, shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			if(enableCaching)
			{
				cachedTextures.insert(std::make_pair(fileName, *shaderResourceView));
			}

			texture2D->Release();
			delete outRGBA;
		}

	}


	void MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value, UINT dimensions, bool forceSRGB, bool enableCaching)
	{
		std::stringstream filename;
		filename << "dummy_texture" << "." << std::hex << std::uppercase << value << "." << dimensions << "." << forceSRGB;

		wchar_t fileName[256];
		MultiByteToWideChar(CP_ACP, 0, filename.str().c_str(), static_cast<int>(filename.str().length() + 1), fileName, 256);

		if (auto iterator = cachedTextures.find(fileName); iterator != cachedTextures.end())
		{
			*shaderResourceView = iterator->second.Get();
			(*shaderResourceView)->AddRef();
		}
		else
		{
			HRESULT hr = S_OK;

			ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();

			D3D11_TEXTURE2D_DESC texture2DDesc = {};
			texture2DDesc.Width = dimensions;
			texture2DDesc.Height = dimensions;
			texture2DDesc.MipLevels = 1;
			texture2DDesc.ArraySize = 1;
			if (forceSRGB)
			{
				texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			}
			else
			{
				texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			}
			texture2DDesc.SampleDesc.Count = 1;
			texture2DDesc.SampleDesc.Quality = 0;
			texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texture2DDesc.CPUAccessFlags = 0;
			texture2DDesc.MiscFlags = 0;

			std::unique_ptr<uint32_t[]> sysmem = std::make_unique< uint32_t[]>(dimensions *dimensions);
			for (size_t i = 0; i < dimensions * dimensions; i++)
			{
				sysmem[i] = value;
			}
			D3D11_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pSysMem = sysmem.get();
			subresourceData.SysMemPitch = sizeof(DWORD) * dimensions;
			subresourceData.SysMemSlicePitch = 0;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
			hr = device->CreateTexture2D(&texture2DDesc, &subresourceData, &texture2D);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
			shaderResourceViewDesc.Format = texture2DDesc.Format;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;

			hr = device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			if (hr == S_OK && enableCaching)
			{
				cachedTextures.insert(std::make_pair(fileName, *shaderResourceView));
			}
		}
	}
	void ReleaseAllCachedTextures()
	{
		cachedTextures.clear();
	}
	void ReleaseCachedTexture(const std::string& textureName)
	{
		wchar_t fileName[256];
		MultiByteToWideChar(CP_ACP, 0, textureName.c_str(), static_cast<int>(strlen(textureName.c_str()) + 1), fileName, 256);

		if (const auto iterator = cachedTextures.find(fileName); iterator != cachedTextures.end())
		{
			cachedTextures.erase(fileName);
		}
	}

}

