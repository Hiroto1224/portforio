#pragma once
#include <d3d11.h>

#include "StartingOver.h"

namespace StartingOver
{
	/// <summary>
	/// RenderTargetViewÇ∆depthStencilViewÇ∆ShaderResourceViewÇÃê›íËÇ∆óòóp
	/// </summary>
	class FrameBuffer
	{
	public:
		FrameBuffer(float width, float height, bool enableMsaa = false, int subSamples = 1,
			DXGI_FORMAT renderTargetTexture2DFormat = DXGI_FORMAT_R8G8_B8G8_UNORM, 
			DXGI_FORMAT depthStencilTexture2DfFormat = DXGI_FORMAT_R24G8_TYPELESS,
			bool needRenderTargetShaderResourceView = true,
			bool needDepthStencilShaderResourceView = true, bool generateMips = false);
		FrameBuffer(ID3D11RenderTargetView* renderTargetView,
			DXGI_FORMAT depthStencilTexture2DFormat = DXGI_FORMAT_R24G8_TYPELESS,
			bool needDepthStencilShaderResourceView = true);


		virtual ~FrameBuffer() = default;

	private:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSView;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> renderTargetShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthStencilShaderResourceView;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;

		D3D11_VIEWPORT viewport;
		D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

		UINT storedViewportsNum;

	public:
		void Clear(float r = 0, float g = 0, float b = 0, float a = 1, UINT _clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, float _depth = 1.0f, UINT8 _stencil = 0)
		{
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();
			float color[4] = { r,g,b,a };
			if(RTView)
			{
				context->ClearRenderTargetView(RTView.Get(), color);
			}
			if(DSView)
			{
				context->ClearDepthStencilView(DSView.Get(), _clearFlags, _depth, _stencil);
			}
		}
		void ClearRenderTargetView(float r = 0, float g = 0, float b = 0, float a = 1)
		{
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();
			float color[4] = { r,g,b,a };
			context->ClearRenderTargetView(RTView.Get(), color);
		}
		void ClearDepthStencilView(UINT _clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,float _depth = 1.0f,UINT8 _stencil = 0)
		{
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();
			context->ClearDepthStencilView(DSView.Get(), _clearFlags, _depth, _stencil);
		}
		void Activate()
		{
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

			storedViewportsNum = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
			context->RSGetViewports(&storedViewportsNum, cachedViewports);
			context->RSSetViewports(1, &viewport);

			context->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(),cachedDepthStencilView.ReleaseAndGetAddressOf());
			context->OMSetRenderTargets(1, RTView.GetAddressOf(), DSView.Get());
		}
		void ActiveRenderTargetView()
		{
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

			storedViewportsNum = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
			context->RSGetViewports(&storedViewportsNum, cachedViewports);
			context->RSSetViewports(1, &viewport);

			context->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
			context->OMSetRenderTargets(1, RTView.GetAddressOf(), nullptr);
		}

		void ActiveDepthStencilView()
		{
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

			storedViewportsNum = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
			context->RSGetViewports(&storedViewportsNum, cachedViewports);
			context->RSSetViewports(1, &viewport);

			context->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
			ID3D11RenderTargetView* nullRenderTargetView = nullptr;
			context->OMSetRenderTargets(1, &nullRenderTargetView, DSView.Get());
		}
		void InActivate()
		{
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

			context->RSSetViewports(storedViewportsNum, cachedViewports);
			context->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
		}

		ID3D11ShaderResourceView* GetRenderTargetShaderResourceView()
		{
			return renderTargetShaderResourceView.Get();
		}
	};
}
