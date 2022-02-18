#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "DirectXSystem.h"

namespace StartingOver
{
	class Blender
	{
	public:
		enum BLEND_STATE
		{
			BS_NONE, BS_ALPHA, BS_ADD, BS_SUBTRACT,
			BS_MULTIPLY, BS_SCREEN, BS_END
		};


	private:
		static Microsoft::WRL::ComPtr<ID3D11BlendState> blendState[BS_END];

	public:


		static void setBlendDesc(Microsoft::WRL::ComPtr<ID3D11Device> device)
		{
			// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_blend_desc
		// RESULT_COLOR = DestBlend * DestColor [BlendOp(+ か　-)] SrcBlend * SrcColor
			HRESULT hr = S_OK;
			D3D11_BLEND_DESC blend_Desc = {};
			// BS_NONE　何もしない
			blend_Desc.AlphaToCoverageEnable = FALSE;
			blend_Desc.IndependentBlendEnable = FALSE;
			blend_Desc.RenderTarget[0].BlendEnable = FALSE;
			blend_Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blend_Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			blend_Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_Desc, blendState[BS_NONE].GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
						//	BS_ALPHA　半透明
							// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_blend_desc
						// アルファブレンディングを有効にする 
			blend_Desc.AlphaToCoverageEnable = FALSE;
			blend_Desc.IndependentBlendEnable = FALSE;
			blend_Desc.AlphaToCoverageEnable = FALSE;
			blend_Desc.IndependentBlendEnable = FALSE;
			blend_Desc.RenderTarget[0].BlendEnable = TRUE;
			blend_Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blend_Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blend_Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_Desc, blendState[BS_ALPHA].GetAddressOf());
			//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
				//	BS_ADD　加算
			blend_Desc.AlphaToCoverageEnable = FALSE;
			blend_Desc.IndependentBlendEnable = FALSE;
			blend_Desc.RenderTarget[0].BlendEnable = TRUE;
			blend_Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE; //D3D11_BLEND_ONE
			blend_Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blend_Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			blend_Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blend_Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_Desc, blendState[BS_ADD].GetAddressOf());
			//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
				// BS_SUBTRACT 減算
			blend_Desc.AlphaToCoverageEnable = FALSE;
			blend_Desc.IndependentBlendEnable = FALSE;
			blend_Desc.RenderTarget[0].BlendEnable = TRUE;
			blend_Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_SRC_ALPHA
			blend_Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE; //D3D11_BLEND_ONE
			blend_Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT; //D3D11_BLEND_OP_SUBTRACT
			blend_Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			blend_Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blend_Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_Desc, blendState[BS_SUBTRACT].GetAddressOf());
			//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
				//	BS_MULTIPLY　乗算
			blend_Desc.AlphaToCoverageEnable = FALSE;
			blend_Desc.IndependentBlendEnable = FALSE;
			blend_Desc.RenderTarget[0].BlendEnable = TRUE;
			blend_Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_DEST_COLOR
			blend_Desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR; //D3D11_BLEND_SRC_COLOR
			blend_Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
			blend_Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_Desc, blendState[BS_MULTIPLY].GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			//	BS_SCREEN　スクリーン合成
			blend_Desc.AlphaToCoverageEnable = FALSE;
			blend_Desc.IndependentBlendEnable = FALSE;
			blend_Desc.RenderTarget[0].BlendEnable = TRUE;
			blend_Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3DBLEND_INVDESTCOLOR
			blend_Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR; //D3DBLEND_ONE
			blend_Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blend_Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_Desc, blendState[BS_SCREEN].GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		}

		static Microsoft::WRL::ComPtr<ID3D11BlendState> GetBlendState(const BLEND_STATE blend)
		{
			return blendState[blend].Get();
		}
	};
}