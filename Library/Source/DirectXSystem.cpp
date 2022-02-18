#include <windows.h>
#include <d3d11.h>
#include "../Header/DirectXSystem.h"
#include "../Header/misc.h"
#include "../Header/Blender.h"
#define MSAA 0

namespace StartingOver
{
	Microsoft::WRL::ComPtr<ID3D11Device>			DirectX11Manager::device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		DirectX11Manager::context;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			DirectX11Manager::swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  DirectX11Manager::renderTargetView;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	DirectX11Manager::depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			DirectX11Manager::texture2D;
	HighResolutionTimer								DirectX11Manager::hrTimer;
	D3D11_TEXTURE2D_DESC							DirectX11Manager::textureDesc;
	float											DirectX11Manager::screenHeight;
	float											DirectX11Manager::screenWidth;
	Microsoft::WRL::ComPtr<ID3D11BlendState>		Blender::blendState[BS_END];

	bool DirectX11Manager::Initialize(const HWND hWnd, const float setScreenWidth, const float setScreenHeight, const bool fullScreen, const double frameRate)
	{
		screenWidth = setScreenWidth;
		screenHeight = setScreenHeight;
		CreateDevice(hWnd, fullScreen);
		InitializeRenderTarget(hWnd);
		Blender::setBlendDesc(device.Get());
		hrTimer.setFrameRate(frameRate);

		return true;
	}
	HRESULT DirectX11Manager::CreateDevice(const HWND hWnd, const bool fullScreen)
	{
		HRESULT hr = S_OK;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if (FAILED(hr = D3D11CreateDevice(
			// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdevice
			//　ここではデバイスの初期化をするから大体は0かNULLで当たり前
			NULL,	// IDXGIAdapterインターフェースは、ディスプレイサブシステム（1つ以上のGPU、DAC、ビデオメモリを含む）を表します。
			D3D_DRIVER_TYPE_HARDWARE, // D3D_DRIVER_TYPE ドライバーの種類のオプション
			NULL, // Software ソフトウェアラスタライザーを実装するDLLへのハンドル
			0, // Flags 値を有効にするためのランタイムレイヤーは、ビット単位でORを一緒にすることができます。
			featureLevelArray, // 作成しようとする機能レベルの順序を決定するD3D_FEATURE_LEVELの配列へのポインター。
			2, // pFeatureLevelsの要素数
			D3D11_SDK_VERSION, // SDKバージョン。D3D11_SDK_VERSIONを使用する。
			device.GetAddressOf(), // 作成したデバイスのオブジェクトへのポインターのアドレスを返す
			&featureLevel,	//　成功した場合、成功したpFeatureLevels配列から最初のD3D_FEATURE_LEVELを返します。 
					//　どの機能レベルがサポートされているかを判断する必要がない場合は、入力としてNULLを指定します。
			context.GetAddressOf() //　作成したデバイスコンテキストオブジェクトへのポインターのアドレスを返す
		)))
		{
			MessageBoxW(hWnd, L"デバイスの作成に失敗(beginning_System)", L"ERR", MB_ICONSTOP);
			return hr;
		}

#if MSAA 
		DXGI_SAMPLE_DESC sampleDesc;
		ZeroMemory(&sampleDesc, sizeof(sampleDesc));
		for (int i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i <<= 1)
		{
			UINT Quality;
			hr = device->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality);
			if (SUCCEEDED(hr))
			{
				if (0 < Quality)
				{
					sampleDesc.Count = i;
					sampleDesc.Quality = Quality - 1;
				}
			}
		}
#endif // MSAA



		//インターフェース取得
		Microsoft::WRL::ComPtr <IDXGIDevice> dxgi = nullptr;
		// https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(refiid_void)
		hr = device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgi.GetAddressOf()));
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))	return hr;

		//アダプター取得
		Microsoft::WRL::ComPtr <IDXGIAdapter> adapter;
		// https://docs.microsoft.com/ja-jp/windows/win32/api/dxgi/nf-dxgi-idxgidevice-getadapter
		hr = dxgi->GetAdapter(adapter.GetAddressOf());
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))	return hr;

		////ファクトリー取得
		Microsoft::WRL::ComPtr <IDXGIFactory> factory;
		// https://docs.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-getparent
		// 指定したウィンドウの親または所有者へのハンドルを取得します。
		hr = adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf()));
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))	return hr;



		// DXGI_SWAP_CHAIN_DESC SwapChain構造体
		DXGI_SWAP_CHAIN_DESC swapChain_Desc;
		// 解像度の縦幅と横幅を設定する
		swapChain_Desc.BufferDesc.Width = static_cast<UINT>(screenWidth);
		swapChain_Desc.BufferDesc.Height = static_cast<UINT>(screenHeight);
		// リフレッシュレートをヘルツ単位で記述
		swapChain_Desc.BufferDesc.RefreshRate.Numerator = 0;
		swapChain_Desc.BufferDesc.RefreshRate.Denominator = 0;
		// 表示フォーマットを記述
		// アルファを含むチャネルごとに8ビットをサポートする4コンポーネントの32ビット符号なし正規化整数sRGB形式。
		swapChain_Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// スキャンライン描画モードを記述する列挙型 
		// DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED 指定なし
		swapChain_Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		// スケーリングモードを記述する列挙型
		// DXGI_MODE_SCALING_UNSPECIFIED 指定なし
		swapChain_Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		//　マルチサンプリングパラメータを記述する DXGI_SAMPLE_DESC 構造体。
#if MSAA
		swapChain_Desc.SampleDesc = sampleDesc; // MSAA

#elif !MSAA 
		swapChain_Desc.SampleDesc.Count = 1;  // １ピクセルあたりのマルチサンプル数。
		swapChain_Desc.SampleDesc.Quality = 0; //画質レベルのこと。画質が高いほどパフォーマンスが低下します。有効な範囲は0～１
#endif 
	// DXGI_USAGE 列挙型のメンバで、バック バッファのサーフェイス使用量と CPU アクセス オプションを記述します。
	// バックバッファは、シェーダ入力またはレンダーターゲット出力に使用することができます。
		swapChain_Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // サーフェスまたはリソースを出力レンダーターゲットとして使用します。
		// スワップチェーン内のバッファの数を表す値。
		swapChain_Desc.BufferCount = 1;
		// 出力ウィンドウへのHWNDハンドル。 このメンバーはNULLであってはなりません。
		swapChain_Desc.OutputWindow = hWnd;
		// 出力がウィンドウモードであるかどうかを指定するブール値 出力がウィンドウモードの場合はTRUE それ以外の場合、FALSE
		swapChain_Desc.Windowed = TRUE;
		// サーフェス提示後のプレゼンテーションバッファの内容を処理するオプションを記述した DXGI_SWAP_EFFECT 列挙型のメンバ
		swapChain_Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		// DXGI_SWAP_CHAIN_FLAG 列挙型のメンバで、スワップチェーン動作のオプションを記述する
		swapChain_Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgifactory-createswapchain
		hr = factory->CreateSwapChain(device.Get(), &swapChain_Desc, swapChain.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBoxW(hWnd, L"CreateSwapChain(beginning_System)", L"ERR", MB_ICONSTOP);
			return hr;
		}

		if (fullScreen)
		{
			swapChain->SetFullscreenState(TRUE, nullptr);
		}

		return hr;
	}


	//
	//
	//
	//
	//
	//
	bool DirectX11Manager::CreateDepthStencil(const HWND hWnd)
	{
		HRESULT hr = S_OK;
		// DepthStencilViewの作成
		// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_texture2d_desc
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = static_cast<UINT>(screenWidth);
		textureDesc.Height = static_cast<UINT>(screenHeight);
		// テクスチャ内のミップマップレベルの最大数
		// ミップマップの説明　https://yttm-work.jp/gmpg/gmpg_0018.html
		textureDesc.MipLevels = 1;
		// テクスチャ配列内のテクスチャの数
		textureDesc.ArraySize = 1;
		// HACK: 深度ステンシルとして利用できるものは４つでわけがわからないので違いが判る段階で切り替える
		/*
		DXGI_FORMAT_D16_UNORM
		DXGI_FORMAT_D24_UNORM_S8_UINT
		DXGI_FORMAT_D32_FLOAT
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT
		*/
		textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
		// Sampledes https://docs.microsoft.com/ja-jp/windows/win32/api/dxgicommon/ns-dxgicommon-dxgi_sample_desc
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		//texture_Desc.SampleDesc = sampleDesc;
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_usage 一般的に使われるのはD3D11_USAGE_DEFAULT
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		// パイプラインステージにバインドするためのフラグ
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_bind_flag
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		// 許可されるCPUアクセスのタイプを指定するフラグ CPUアクセスが不要な場合は0を使用
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_cpu_access_flag
		textureDesc.CPUAccessFlags = 0;
		// あまり一般的ではない他のリソースオプションを識別するフラグ
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_resource_misc_flag
		textureDesc.MiscFlags = 0;
		// CreateTexture2D
		// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createtexture2d
		if (FAILED(hr = device->CreateTexture2D(&textureDesc, NULL, texture2D.GetAddressOf())))
		{
			MessageBoxW(hWnd, L"深度ステンシルバッファの作成に失敗beginning_System)", L"ERR", MB_ICONSTOP);
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsDESC = {};
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_depth_stencil_view_desc
		dsDESC.Format = textureDesc.Format;
		// リソースのタイプ 深度ステンシルリソースへのアクセス方法を指定する 値はこの構造体の共用体に格納される
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_dsv_dimension
		dsDESC.ViewDimension = textureDesc.SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		// 使用する最初のミップマップレベルのインデックス
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_tex2d_dsv
		dsDESC.Texture2D.MipSlice = 0;
		dsDESC.Flags = 0;
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/nf-d3d11-id3d11device-createdepthstencilview
		if (FAILED(hr = device->CreateDepthStencilView(texture2D.Get(), &dsDESC, depthStencilView.GetAddressOf())))
		{
			MessageBoxW(hWnd, L"深度ステンシルビューの作成に失敗(beginning_System)", L"ERR", MB_ICONSTOP);
			return false;
		}

		return true;
	}


	bool DirectX11Manager::InitializeRenderTarget(const HWND hWnd)
	{

		HRESULT hr = S_OK;
		if (FAILED(hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), static_cast<void**>(&texture2D))))
		{
			MessageBoxW(hWnd, L"バックバッファの取得に失敗(beginning_System)", L"ERR", MB_ICONSTOP);
			return false;
		}
		// レンダーターゲットビューの作成
		// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createrendertargetview
		if (FAILED(hr = device->CreateRenderTargetView(texture2D.Get(), NULL, renderTargetView.GetAddressOf())))
		{
			MessageBoxW(hWnd, L"レンダーターゲットビューの作成に失敗(beginning_System)", L"ERR", MB_ICONSTOP);
			return false;
		}

		CreateDepthStencil(hWnd);

		context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		SetViewPort(screenWidth, screenHeight);

		return true;
	}


	void DirectX11Manager::SetViewPort(const float width, const float height)
	{
		// ビューポートの設定
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_viewport
		D3D11_VIEWPORT Viewport;
		// ビューポートの幅
		Viewport.Width = width;
		// ビューポートの高さ
		Viewport.Height = height;
		// ビューポートの最小深度
		Viewport.MinDepth = 0.0f;
		// ビューポートの最大深度
		Viewport.MaxDepth = 1.0f;
		// ビューポートの左側のX位置 つまり０
		Viewport.TopLeftX = 0.0f;
		// ビューポートの上部のY位置 つまり０
		Viewport.TopLeftY = 0.0f;
		context->RSSetViewports(1, &Viewport);
	}

}

