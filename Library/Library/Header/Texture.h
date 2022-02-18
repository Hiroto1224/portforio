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




class Texture
{
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		 sampler = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   RTView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   DSView = nullptr;
	// テクスチャ情報
	D3D11_TEXTURE2D_DESC tex2dDesc;

public:
	bool is_Texture;

public:
	Texture() = default;
	virtual ~Texture(){};
	bool Load(const std::string& filename, int samplerState = 0);
	bool LoadMipMap(const std::string* mipMapName = nullptr);
	bool MakeDummyTexture(ID3D11Device* device);
	void Set(UINT slot = 0, BOOL flg = TRUE);
	UINT GetWidth() { return tex2dDesc.Width; }
	UINT GetHeight() { return tex2dDesc.Height; }
	bool Create(u_int width, u_int height, DXGI_FORMAT format);


	// Create DeothStencilBuffer
	bool CreateDepth(u_int width, u_int height, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS);
		
	ID3D11RenderTargetView* GetRenderTarget() { return RTView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() { return DSView.Get(); }
	ID3D11ShaderResourceView** GetSRView() { return SRView.GetAddressOf(); }

};