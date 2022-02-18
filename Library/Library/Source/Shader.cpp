#include <d3dcompiler.h>
#include "../Header/DirectXSystem.h"
#include "../Header/Shader.h"
#include "../Header/misc.h"
#include <cassert>
#include <filesystem>
#include <map>

namespace StartingOver
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>> cachedInputLayouts;

	HRESULT CreateInputLayout(ID3D11InputLayout** _inputLayout, D3D11_INPUT_ELEMENT_DESC* _inputElementDesc, size_t _elementsNum, bool _enableCaching)
	{
		std::string key;
		std::string source;
		source.append("struct VSInput\n");
		source.append("{\n");
		ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();


		for (size_t i = 0; i < _elementsNum; i++)
		{
			std::stringstream line;
			switch (_inputElementDesc[i].Format)
			{
			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R32G32_FLOAT:
			case DXGI_FORMAT_R32G32B32_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
				line << "float4";
				key.append(_inputElementDesc[i].SemanticName + std::to_string(_inputElementDesc[i].Format));
				break;
			case DXGI_FORMAT_R32_UINT:
			case DXGI_FORMAT_R32G32_UINT:
			case DXGI_FORMAT_R32G32B32_UINT:
			case DXGI_FORMAT_R32G32B32A32_UINT:
				line << "uint4";
				key.append(std::to_string(_inputElementDesc[i].Format) + ":");
				break;
			case DXGI_FORMAT_R32_SINT:
			case DXGI_FORMAT_R32G32_SINT:
			case DXGI_FORMAT_R32G32B32_SINT:
			case DXGI_FORMAT_R32G32B32A32_SINT:
				line << "int4 ";
				key.append(std::to_string(_inputElementDesc[i].Format) + ":");
				break;
			default:
				_ASSERT_EXPR_A(0, "not support this DXGI_FORMAT_XXX");
				break;
			}

			line << " _" << _inputElementDesc[i].SemanticName << " : " << _inputElementDesc[i].SemanticName << " ;\n";
			source.append(line.str());
		}

		source.append("};\n");
		source.append("void main(VSInput input){}\n");

		std::map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>>::iterator it = cachedInputLayouts.find(key);
		if (it != cachedInputLayouts.end())
		{
			*_inputLayout = it->second.Get();
			(*_inputLayout)->AddRef();
			return S_OK;
		}

		HRESULT hr = S_OK;
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

		Microsoft::WRL::ComPtr<ID3D11VertexShader> dummyVertexShader;
		Microsoft::WRL::ComPtr<ID3DBlob> compiledShaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorMessageBlob;

		const std::string target = "vs_5_0";
		hr = D3DCompile(source.c_str(), source.length(), 0, 0, 0, "main", target.c_str(), flags, 0, compiledShaderBlob.GetAddressOf(), errorMessageBlob.GetAddressOf());
		_ASSERT_EXPR_A(SUCCEEDED(hr), reinterpret_cast<LPCSTR>(errorMessageBlob->GetBufferPointer()));
		hr = device->CreateVertexShader(compiledShaderBlob->GetBufferPointer(), compiledShaderBlob->GetBufferSize(), 0, dummyVertexShader.GetAddressOf());
		_ASSERT_EXPR_A(SUCCEEDED(hr), reinterpret_cast<LPCSTR>(errorMessageBlob->GetBufferPointer()));
		hr = device->CreateInputLayout(_inputElementDesc, _elementsNum, compiledShaderBlob->GetBufferPointer(), compiledShaderBlob->GetBufferSize(), _inputLayout);
		_ASSERT_EXPR_A(SUCCEEDED(hr), reinterpret_cast<LPCSTR>(errorMessageBlob->GetBufferPointer()));

		if(_enableCaching)
		{
			cachedInputLayouts.insert(std::make_pair(key, *_inputLayout));
		}
		return hr;
	}

	void ReleaseAllCachedInputLayouts()
	{
		cachedInputLayouts.clear();
	}

	struct VertexShaderAndInputLayout
	{
		VertexShaderAndInputLayout(ID3D11VertexShader* _vertexShader, ID3D11InputLayout* _inputLayout) :
			vertexShader(_vertexShader), inputLayout(_inputLayout) {};

		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	};
	static std::map<std::string, VertexShaderAndInputLayout> cachedVertexShaders;

	HRESULT CreateVSFromCSO(const std::string _csoFileName, ID3D11VertexShader** _vertexShader, ID3D11InputLayout** _inputLayout, D3D11_INPUT_ELEMENT_DESC* _inputElementDesc, size_t _elementsNum, bool _enableCaching)
	{
		std::map<std::string, VertexShaderAndInputLayout>::iterator it = cachedVertexShaders.find(_csoFileName);

		if (it != cachedVertexShaders.end())
		{
			*_vertexShader = it->second.vertexShader.Get();
			(*_vertexShader)->AddRef();
			if (_inputLayout)
			{
				*_inputLayout = it->second.inputLayout.Get();
				_ASSERT_EXPR_A(*_inputLayout, "cached inputLayout must be not nil");
				(*_inputLayout)->AddRef();
			}
			return S_OK;
		}

		FILE* fp = 0;
		fopen_s(&fp, _csoFileName.c_str(), "rb");
		_ASSERT_EXPR_A(fp, "cso File not Found");

		fseek(fp, 0, SEEK_END);
		long fPos = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		std::unique_ptr<uint8_t[]> csoData = std::make_unique<uint8_t[]>(fPos);
		fread(csoData.get(), fPos, 1, fp);
		fclose(fp);

		ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();

		HRESULT hr = S_OK;
		hr = device->CreateVertexShader(csoData.get(),fPos,0,_vertexShader);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (_inputLayout)
		{
			hr = device->CreateInputLayout(_inputElementDesc, _elementsNum, csoData.get(), fPos, _inputLayout);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		if (_enableCaching)
		{
			if (_inputLayout)
			{
				cachedVertexShaders.insert(std::make_pair(_csoFileName, VertexShaderAndInputLayout(*_vertexShader, *_inputLayout)));
			}else
			{
				cachedVertexShaders.insert(std::make_pair(_csoFileName, VertexShaderAndInputLayout(*_vertexShader, 0)));
			}
		}
		return hr;
	}
	void ReleaseAllCachedVertexShaders()
	{
		cachedVertexShaders.clear();

	}


	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> cachedPixelShaders;
	HRESULT CreatePSFromCSO(const std::string _csoName, ID3D11PixelShader** _pixelShader, ID3D11ClassLinkage** _classLinkage, ID3D11ShaderReflection** _reflector, bool _enable_caching)
	{
		std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>::iterator it = cachedPixelShaders.find(_csoName);

		if (it != cachedPixelShaders.end())
		{
			*_pixelShader = it->second.Get();
			(*_pixelShader)->AddRef();
			return S_OK;
		}

		FILE* fp = 0;
		fopen_s(&fp, _csoName.c_str(), "rb");
		_ASSERT_EXPR_A(fp, "cso File not Found");

		fseek(fp, 0, SEEK_END);
		long fPos = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		std::unique_ptr<uint8_t[]> csoData = std::make_unique<uint8_t[]>(fPos);
		fread(csoData.get(), fPos, 1, fp);
		fclose(fp);

		ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();

		HRESULT hr = S_OK;
		if(_classLinkage)
		{
			hr = device->CreateClassLinkage(_classLinkage);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			hr = device->CreatePixelShader(csoData.get(), fPos, *_classLinkage, _pixelShader);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		if(_reflector)
		{
			hr = D3DReflect(csoData.get(), fPos, IID_ID3D11ShaderReflection, reinterpret_cast<void**>(_reflector));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		hr = device->CreatePixelShader(csoData.get(), fPos, 0, _pixelShader);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if(_enable_caching)
		{
			cachedPixelShaders.insert(std::make_pair(_csoName, *_pixelShader));
		}
		return hr;
	}

	void ReleaseAllCachedPixelShaders()
	{
		cachedPixelShaders.clear();
	}

}