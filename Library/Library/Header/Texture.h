#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <string>

#include "misc.h"


namespace StartingOver
{
	void LoadTextureFromFile(const std::string& textureName, ID3D11ShaderResourceView** _shaderResourceView, bool _forceSRGB, bool _enableCaching);
	void LoadHdrTextureFromFile(const std::string& hdrTextureName, ID3D11ShaderResourceView** shaderResourceView, bool enableCaching);
	void MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value, UINT dimensions, bool forceSRGB, bool enableCaching);
	void ReleaseAllCachedTextures();
	void ReleaseCachedTexture(const std::string& textureName);

	inline void Texture2DDescription(ID3D11ShaderResourceView* _shaderResourceView, D3D11_TEXTURE2D_DESC& _texture2DDesc)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		_shaderResourceView->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
		HRESULT hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2D.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		texture2D->GetDesc(&_texture2DDesc);
	}

}


