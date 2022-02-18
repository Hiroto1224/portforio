#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "StartingOver.h"

namespace StartingOver
{
	class SamplerState
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
		SamplerState(const D3D11_FILTER filter = D3D11_FILTER_ANISOTROPIC, const D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_WRAP,
		             const D3D11_COMPARISON_FUNC comparisonFunc = D3D11_COMPARISON_NEVER, const float r = 1, const float g = 1, const float b = 1, const float a = 1)
		{
			ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();
			D3D11_SAMPLER_DESC samplerDesc = {};
			samplerDesc.Filter = filter;
			samplerDesc.AddressU = addressMode;
			samplerDesc.AddressV = addressMode;
			samplerDesc.AddressW = addressMode;
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = comparisonFunc;
			const float borderColor[4] = { r,g,b,a };
			memcpy(samplerDesc.BorderColor, borderColor, sizeof(float) * 4);
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			HRESULT hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		SamplerState(const D3D11_SAMPLER_DESC* samplerDesc)
		{
			ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();

			HRESULT hr = device->CreateSamplerState(samplerDesc, samplerState.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		~SamplerState() = default;

		void Activate(UINT _slot, bool _setVS = false)
		{
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();
			usingSlot = _slot;

			context->PSGetSamplers(usingSlot, 1, cachedSamplerState[0].ReleaseAndGetAddressOf());
			context->VSSetSamplers(usingSlot, 1, samplerState.GetAddressOf());
			context->PSSetSamplers(usingSlot, 1, samplerState.GetAddressOf());

			if (_setVS)
			{
				context->VSGetSamplers(usingSlot, 1, cachedSamplerState[1].ReleaseAndGetAddressOf());
			}
		}
		void InActivate()
		{
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();
			context->PSSetSamplers(usingSlot, 1, cachedSamplerState[0].GetAddressOf());
			context->VSSetSamplers(usingSlot, 1, cachedSamplerState[1].GetAddressOf());
		}
		UINT usingSlot = 0;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> cachedSamplerState[2];
	};
}