#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl.h>

#include "high_resolution_timer.h"


namespace StartingOver
{

	class DirectX11Manager
	{
	public:
		DirectX11Manager() = default;

	private:
		static float screenWidth;
		static float screenHeight;
		static D3D11_TEXTURE2D_DESC textureDesc;
		static Microsoft::WRL::ComPtr<ID3D11Device> device;
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
		static Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
		static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
		static Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
		static HRESULT CreateDevice(HWND hWnd, bool fullScreen);

		static bool CreateDepthStencil(HWND hWnd);
		static bool InitializeRenderTarget(HWND hWnd);
		static void SetViewPort(float width, float height);

	public:
		static bool Initialize(HWND hWnd, float setScreenWidth, float setScreenHeight, bool fullScreen, double frameRate);

		static float GetScreenWidth() { return screenWidth; }
		static float GetScreenHeight() { return screenHeight; }
		static ID3D11Device* GetDevice() { return device.Get(); }
		static ID3D11DeviceContext* GetDeviceContext() { return context.Get(); }
		static IDXGISwapChain* GetSwapChain() { return swapChain.Get(); }
		static ID3D11DepthStencilView* GetDepthStencilView() { return depthStencilView.Get(); }
		static ID3D11RenderTargetView* GetRenderTargetView() { return renderTargetView.Get(); }
		static ID3D11Texture2D* GetTexture2D() { return texture2D.Get(); }
		static HighResolutionTimer hrTimer;

		

		
	};


}