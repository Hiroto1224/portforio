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


bool Texture::Load(const std::string& filename,int samplerState)
{
	HRESULT hr = S_OK;

	// シェーダーリソースビューの作成
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	setlocale(LC_ALL, "japanese");
	wchar_t fileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, fileName, MAX_PATH, filename.c_str(), _TRUNCATE);
	

	static std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;
	if (const auto iterator = cache.find(fileName); iterator != cache.end())
	{
		SRView = iterator->second.Get();
		SRView->AddRef();
		SRView->GetResource(resource.GetAddressOf());
		is_Texture = true;
	}
	else
	{
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;
		u_int textureFlg = 0;

		std::wstring extension = PathFindExtensionW(fileName);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
		//WIC includes several built - in codecs.The following standard codecs are provided with the platform.
		//	Codec																	Mime Types								Decoders	Encoders
		//	BMP(Windows Bitmap Format), BMP Specification v5.						image / bmp								Yes			Yes
		//	GIF(Graphics Interchange Format 89a), GIF Specification 89a / 89m		image / gif								Yes			Yes
		//	ICO(Icon Format)														image / ico								Yes			No
		//	JPEG(Joint Photographic Experts Group), JFIF Specification 1.02			image / jpeg, image / jpe, image / jpg	Yes			Yes
		//	PNG(Portable Network Graphics), PNG Specification 1.2					image / png								Yes			Yes
		//	TIFF(Tagged Image File Format), TIFF Specification 6.0					image / tiff, image / tif				Yes			Yes
		//	Windows Media Photo, HD Photo Specification 1.0							image / vnd.ms - phot					Yes			Yes
		//	DDS(DirectDraw Surface)													image / vnd.ms - dds					Yes			Yes
		if (L".png" == extension || L".jpeg" == extension || L".jpg" == extension || L".jpe" == extension || L".gif" == extension || L".tiff" == extension || L".tif" == extension || L".bmp" == extension)
		{
			hr = LoadFromWICFile(fileName, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &metadata, image);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else if (L".dds" == extension)
		{
			hr = LoadFromDDSFile(fileName, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &metadata, image);
			textureFlg = D3D11_RESOURCE_MISC_TEXTURECUBE;
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else if (L".tga" == extension || L".vda" == extension || L".icb" == extension || L".vst" == extension)
		{
			hr = LoadFromTGAFile(fileName, &metadata, image);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else
		{
			_ASSERT_EXPR(false, L"File format not supported.");
		}
		
		if (hr == S_OK)
		{
			hr = CreateShaderResourceViewEx(
				StartingOver::DirectX11Manager::GetDevice(),
				image.GetImages(),
				image.GetImageCount(),
				metadata,
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_SHADER_RESOURCE,
				0,
				textureFlg,
				false/*force_srgb*/,
				SRView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		SRView->GetResource(resource.GetAddressOf());
		cache.insert(std::make_pair(fileName, SRView));
	}

	// テクスチャ情報
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	resource.Get()->QueryInterface(texture2D.GetAddressOf());
	texture2D.Get()->GetDesc(&tex2dDesc);

	// サンプラーステート作成
	D3D11_SAMPLER_DESC sd = {};
	if (samplerState == 0)
	{
		sd.Filter = D3D11_FILTER_ANISOTROPIC; // 異方性フィルタ
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MipLODBias = 0;
		sd.MaxAnisotropy = 16;
		sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
	}

	if (samplerState == 1)
	{
		sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.BorderColor[0] = 1.0f;
		sd.BorderColor[1] = 1.0f;
		sd.BorderColor[2] = 1.0f;
		sd.BorderColor[3] = 1.0f;
		sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		sd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		sd.MaxAnisotropy = 16;
		sd.MipLODBias = 0;
		sd.MinLOD = -FLT_MAX;
		sd.MaxLOD = +FLT_MAX;
	}
	hr = StartingOver::DirectX11Manager::GetDevice()->CreateSamplerState(
		&sd, sampler.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	is_Texture = true;
	return true;
}

bool Texture::LoadMipMap(const std::string* mipMapName)
{
	ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();
	HRESULT hr = S_OK;
	wchar_t	wchar[256];
	size_t wLen = 0;
	errno_t err = 0;

	setlocale(LC_ALL, "japanese");
	wchar_t fileName[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, fileName, MAX_PATH, mipMapName->c_str(), _TRUNCATE);

	// 画像ファイル読み込み DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	if (_wcsnicmp(&fileName[wcslen(fileName) - 3], L"TGA", 3) == 0)
	{
		hr = LoadFromTGAFile(fileName, &metadata, image);
	}
	else 
	{
		hr = LoadFromWICFile(fileName, DirectX::WIC_FLAGS_NONE, &metadata, image);
	}

	if (FAILED(hr)) {
		return false;
	}

	//テクスチャデータ取得
	tex2dDesc.Width = metadata.width;
	tex2dDesc.Height = metadata.height;

	//
	DirectX::ScratchImage mipChain;
	DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);

	// 画像からシェーダリソースView
	hr = CreateShaderResourceView(device, mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), &SRView);
	if (FAILED(hr)) {
		return false;
	}
}


void Texture::Set(const UINT slot, const BOOL flg)
{
	if (flg == FALSE)
	{
		ID3D11ShaderResourceView* rtv[1] = { nullptr };
		ID3D11SamplerState* ss[1] = { nullptr };
		StartingOver::DirectX11Manager::GetDeviceContext()->PSSetShaderResources(slot, 1, rtv);
		StartingOver::DirectX11Manager::GetDeviceContext()->PSSetSamplers(slot, 1, ss);
		StartingOver::DirectX11Manager::GetDeviceContext()->DSSetShaderResources(slot, 1, rtv);
		StartingOver::DirectX11Manager::GetDeviceContext()->DSSetSamplers(slot, 1, ss);
		return;
	}
	if (SRView)
	{
		StartingOver::DirectX11Manager::GetDeviceContext()->PSSetShaderResources(slot, 1, SRView.GetAddressOf());
		StartingOver::DirectX11Manager::GetDeviceContext()->PSSetSamplers(slot, 1, sampler.GetAddressOf());
		StartingOver::DirectX11Manager::GetDeviceContext()->DSSetShaderResources(slot, 1, SRView.GetAddressOf());
		StartingOver::DirectX11Manager::GetDeviceContext()->DSSetSamplers(slot, 1, sampler.GetAddressOf());
	}
}


bool Texture::Create(u_int width, u_int height, DXGI_FORMAT format)
{
	ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	
	HRESULT hr = S_OK;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));
	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.Format = format;
	tex2dDesc.SampleDesc.Count = 1;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;
	tex2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tex2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&tex2dDesc, NULL, texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	renderTargetViewDesc.Format = format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(texture2D.Get(), &renderTargetViewDesc, RTView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, SRView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;
}

bool Texture::CreateDepth(u_int width, u_int height, DXGI_FORMAT format)
{
	ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();
	
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;

	HRESULT hr = S_OK;

	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));
	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.Format = format;
	tex2dDesc.SampleDesc.Count = 1;
	tex2dDesc.SampleDesc.Quality = 0;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;
	tex2dDesc.CPUAccessFlags = 0;
	tex2dDesc.MiscFlags = 0;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&tex2dDesc, NULL, texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(texture2D.Get(), &depthStencilViewDesc, DSView.GetAddressOf());
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, SRView.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;

}


bool Texture::MakeDummyTexture(ID3D11Device* device)
{

	HRESULT hr = S_OK;
	constexpr unsigned int dimensions = 1;
	tex2dDesc.Width = dimensions;
	tex2dDesc.Height = dimensions;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex2dDesc.SampleDesc.Count = 1;
	tex2dDesc.SampleDesc.Quality = 0;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;
	tex2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex2dDesc.CPUAccessFlags = 0;
	tex2dDesc.MiscFlags = 0;

	const auto sysMem = new unsigned int[dimensions * dimensions];
	for (unsigned int i = 0; i < dimensions * dimensions; i++)
	{
		sysMem[i] = 0xFFFFFFFF;
	}
	D3D11_SUBRESOURCE_DATA sResourceData = {};
	sResourceData.pSysMem = sysMem;
	sResourceData.SysMemPitch = sizeof(unsigned int);
	sResourceData.SysMemSlicePitch = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	hr = device->CreateTexture2D(&tex2dDesc, &sResourceData, texture2D.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = tex2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, SRView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	texture2D->Release();
	delete[] sysMem;

	return true;
}
