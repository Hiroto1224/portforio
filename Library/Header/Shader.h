#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <string>
#include "StartingOver.h"
#include "DirectXSystem.h"
namespace StartingOver
{
	HRESULT CreateInputLayout(ID3D11InputLayout** _inputLayout, D3D11_INPUT_ELEMENT_DESC* _inputElementDesc, size_t _elementsNum, bool _enableCaching = true);
	void ReleaseAllCachedInputLayouts();
	HRESULT CreateVSFromCSO(const std::string _csoFileName,
		ID3D11VertexShader** _vertexShader, ID3D11InputLayout** _inputLayout,
		D3D11_INPUT_ELEMENT_DESC* _inputElementDesc, size_t _elementsNum, bool enableCaching = true);
	void ReleaseAllCachedVertexShaders();
	HRESULT CreatePSFromCSO(const std::string csoName,
		ID3D11PixelShader** pixelShader, ID3D11ClassLinkage** classLinkage = 0,
		ID3D11ShaderReflection** reflector = 0, bool enable_caching = true);	void ReleaseAllCachedVertexShaders();
	void ReleaseAllCachedPixelShaders();

	template<class T>
	class VertexShader
	{
	public:

		VertexShader(const std::string csoFileName)
		{
			size_t elementsNum;
			const D3D11_INPUT_ELEMENT_DESC* inputElementDesc = T::AcquireInputElementDesc(elementsNum);
			
			if (csoFileName.data())
			{

#if 0
				CreateVSFromCSO(cosFileName, shaderObject.GetAddressOf(), inputLayout.GetAddressOf(), const_cast<D3D11_INPUT_ELEMENT_DESC*>(inputElementDesc), elementsNum);
#else
				CreateVSFromCSO(csoFileName, vertexShader.GetAddressOf(), 0, 0, 0);
				CreateInputLayout(inputLayout.GetAddressOf(), const_cast<D3D11_INPUT_ELEMENT_DESC*>(inputElementDesc), elementsNum);
#endif
			}
		}
		VertexShader(const std::string _csoFileName, D3D11_INPUT_ELEMENT_DESC* _inputElementDesc, size_t _elementsNum, bool _enableCaching)
		{
			if(_csoFileName.data())
			{
				CreateVSFromCSO(_csoFileName, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), const_cast<D3D11_INPUT_ELEMENT_DESC*>(_inputElementDesc), _elementsNum, _enableCaching);
			}
		}

		virtual ~VertexShader() = default;
		VertexShader(VertexShader&) = delete;
		VertexShader& operator=(VertexShader&) = delete;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

		void Activate()
		{
			ID3D11DeviceContext* context = DirectX11Manager::GetDeviceContext();

			context->IAGetInputLayout(cachedInputLayout.ReleaseAndGetAddressOf());
			context->VSGetShader(cachedVertexShader.ReleaseAndGetAddressOf(), 0, 0);

			context->IASetInputLayout(inputLayout.Get());
			context->VSSetShader(vertexShader.Get(), 0, 0);
		}
		void InActivate() const
		{
			ID3D11DeviceContext* context = DirectX11Manager::GetDeviceContext();

			context->IASetInputLayout(cachedInputLayout.Get());
			context->VSSetShader(cachedVertexShader.Get(), 0, 0);
		}
	private:
	

		Microsoft::WRL::ComPtr<ID3D11VertexShader> cachedVertexShader;;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> cachedInputLayout;
	};

	class PixelShader
	{
	public:
		PixelShader(const std::string csoFileName)
		{
			if (csoFileName.data())
			{
				CreatePSFromCSO(csoFileName, pixelShader.GetAddressOf());
			}
		}
		virtual ~PixelShader() = default;
		PixelShader(PixelShader&) = delete;
		PixelShader& operator=(PixelShader&) = delete;

		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

		void Activate()
		{
			ID3D11DeviceContext* context = DirectX11Manager::GetDeviceContext();

			context->PSGetShader(cachedPixelShader.ReleaseAndGetAddressOf(), 0, 0);
			context->PSSetShader(pixelShader.Get(), 0, 0);
		}
		void InActivate() const
		{
			ID3D11DeviceContext* context = DirectX11Manager::GetDeviceContext();

			context->PSSetShader(cachedPixelShader.Get(), 0, 0);
		}
	private:

		Microsoft::WRL::ComPtr<ID3D11PixelShader> cachedPixelShader;
	};
}
