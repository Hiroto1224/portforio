#pragma once
#include <d3d11.h>
#include <wrl.h>


#include "StartingOver.h"

namespace StartingOver
{
	class DepthStencilState
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

		DepthStencilState(const bool depthEnable = true, const D3D11_DEPTH_WRITE_MASK depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL, const D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_LESS)
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			depthStencilDesc.DepthEnable = depthEnable;
			depthStencilDesc.DepthWriteMask = depthWriteMask;
			depthStencilDesc.DepthFunc = depthFunc;
			depthStencilDesc.StencilEnable = false;
			depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();

			HRESULT hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		DepthStencilState(const D3D11_DEPTH_STENCIL_DESC* depthStencilDesc)
		{
			ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();

			HRESULT hr = device->CreateDepthStencilState(depthStencilDesc, depthStencilState.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		~DepthStencilState() = default;

		void Activate()
		{
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();
			context->OMGetDepthStencilState(cachedDepthStencilState.ReleaseAndGetAddressOf(),0);
			context->OMSetDepthStencilState(depthStencilState.Get(), 1);
		}

		void InActivate() const
		{
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

			context->OMSetDepthStencilState(cachedDepthStencilState.Get(), 1);
		}

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> cachedDepthStencilState;

	};
}