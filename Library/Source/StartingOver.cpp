#include "../Header/StartingOver.h"
#include "../Header/DirectXSystem.h"
#include "../Header/Input.h"
#define USE_IMGUI 
#include "../imgui-docking/imgui.h"
#include "../imgui-docking/imgui_impl_win32.h"
#include "../imgui-docking/imgui_impl_dx11.h"

std::unique_ptr<DebugUI> StartingOver::GameManager::debugUi = std::make_unique<DebugUI>();
std::unique_ptr<SceneManager> StartingOver::GameManager::sceneManager = std::make_unique<SceneManager>();

#define ICON_MIN_FA 0xf000
#define ICON_MAX_FA 0xf2e0

namespace StartingOver
{


	void Initialize(const LPCTSTR windowName, const float width, const float height,
	                const bool isFullscreen, const double frameRate)
 	{
		Window::hWnd = Window::WindowInitialize(windowName, width, height);

		DirectX11Manager::Initialize(Window::hWnd, width, height, isFullscreen, frameRate);

		
		// Retrieve a list of available devices
		// Each device name is separated by a single NULL character
		// and the list is terminated with 2 NULL characters
#if _DEBUG
		GameManager::debugUi->Initialize(Window::hWnd);

#endif // _DEBUG
		GameManager::sceneManager->GetActiveScene()->Initialize();
	}

	//--------------------------------------
	//ライブラリの終了処理
	//--------------------------------------
	void UnInitialize()
	{
		GameManager::debugUi->UnInitialize();
		

		Window::WindowUnInitialize();
	}

	//---------------------------------------
	// ゲームループ
	//----------------------------------------
	bool GameLoop(bool isFrameRate)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//フレームレート用
		while (!DirectX11Manager::hrTimer.tick());
		if (isFrameRate)
		{
			CalculateFrameStats(Window::hWnd, &DirectX11Manager::hrTimer);//フレームレート計算・タイトルバーに表示
		}

		ClearDisp(0.0f,0.0f,0.0f);

#if _DEBUG
		GameManager::debugUi->GameLoop();
#endif // _DEBUG

		GameManager::sceneManager->GetActiveScene()->Update();

		return true;
	}

	//--------------------------------------
	//画面のクリア
	//--------------------------------------
	void ClearDisp(float r, float g, float b)
	{
		const float clearColor[] = { r, g, b, 1.0f };
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = DirectX11Manager::GetDeviceContext();
		static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTView = DirectX11Manager::GetRenderTargetView();
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSView = DirectX11Manager::GetDepthStencilView();
		context->ClearRenderTargetView(RTView.Get(), clearColor);
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-cleardepthstencilview
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_clear_flag
		context->ClearDepthStencilView(DSView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	}

	//--------------------------------------
	//次のバックバッファの内容を表示
	//--------------------------------------
	HRESULT Present(const UINT syncInterval, const UINT flags)
	{

		static Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = DirectX11Manager::GetSwapChain();

		GameManager::sceneManager->GetActiveScene()->Render();

#if _DEBUG 
		GameManager::debugUi->Render();

#endif _DEBUG

		const HRESULT hr = swapChain->Present(syncInterval, flags);

		static Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = StartingOver::DirectX11Manager::GetDeviceContext();
		static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTView = StartingOver::DirectX11Manager::GetRenderTargetView();

		ImVec4 clearColor = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);

		context->ClearRenderTargetView(RTView.Get(), reinterpret_cast<float*>(&clearColor));

		return hr;
	}


	void SetBlendMode(const Blender::BLEND_STATE blend)
	{
		Microsoft::WRL::ComPtr<ID3D11BlendState> blendState = Blender::GetBlendState(blend);
		DirectX11Manager::GetDeviceContext()->OMSetBlendState(Blender::GetBlendState(blend).Get(), nullptr, 0xFFFFFFFF);
	}

	//-----------------------------------------------------------
	//
	//					WINDOW
	//
	//-----------------------------------------------------------
	namespace Window
	{

		static constexpr LPCWSTR class_name = L"StartingOver";

		// プロトタイプ宣言
		LRESULT CALLBACK FnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		//--------------------------------------
		//windowの初期化
		//--------------------------------------
		HWND WindowInitialize(const LPCTSTR windowName, const float width, const float height)
		{

#if defined(DEBUG) | defined(_DEBUG)
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
			WNDCLASSEX wcex{};
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = FnWndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = GetModuleHandle(nullptr);
			wcex.hIcon = nullptr;
			wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
			wcex.lpszMenuName = nullptr;
			wcex.lpszClassName = class_name;
			wcex.hIconSm = nullptr;
			RegisterClassEx(&wcex);

			RECT rc = { 0, 0, width, height };
			AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
			HWND hWnd = CreateWindow(class_name,
				windowName,
				WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
				0,
				0,
				width,
				height,
				NULL,
				NULL,
				GetModuleHandle(NULL),
				NULL);

			::ShowWindow(hWnd, SW_SHOWDEFAULT);
			::UpdateWindow(hWnd);
			return hWnd;


		}

		//--------------------------------------
		//windowの終了処理
		//--------------------------------------
		void WindowUnInitialize()
		{
			// windowの終了処理
			UnregisterClass(class_name, GetModuleHandle(NULL));
		}

		//--------------------------------------
		//ウィンドウプロシージャ
		//--------------------------------------
		LRESULT CALLBACK FnWndProc(const HWND hwnd, const UINT msg, const WPARAM wparam, const LPARAM lparam)
		{
			static Microsoft::WRL::ComPtr<ID3D11Device> device = StartingOver::DirectX11Manager::GetDevice();
			static Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = DirectX11Manager::GetSwapChain();

#ifdef _DEBUG
			// true なら早期リターン
			if (GameManager::debugUi->WinProc(hwnd,msg,wparam,lparam)) { return true; }
#endif

			float mouseWheel = Input::GetMouseWheel();

			switch (msg)
			{
			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				BeginPaint(hwnd, &ps);
				EndPaint(hwnd, &ps);
				break;
			}
			case WM_MOUSEWHEEL:
				mouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wparam) / (float)WHEEL_DELTA;
				Input::SetMouseWheel(mouseWheel);
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_CREATE:
				break;
			case WM_KEYDOWN:
				if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
				break;
			case WM_ENTERSIZEMOVE:
				// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
				DirectX11Manager::hrTimer.stop();
				break;
			case WM_EXITSIZEMOVE:
				// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
				// Here we reset everything based on the new window dimensions.
				DirectX11Manager::hrTimer.start();
				break;
			default:
				return DefWindowProc(hwnd, msg, wparam, lparam);
			}
			return 0;
		}
	}


}