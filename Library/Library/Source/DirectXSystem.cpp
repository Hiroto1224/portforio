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
			//�@�����ł̓f�o�C�X�̏����������邩���̂�0��NULL�œ�����O
			NULL,	// IDXGIAdapter�C���^�[�t�F�[�X�́A�f�B�X�v���C�T�u�V�X�e���i1�ȏ��GPU�ADAC�A�r�f�I���������܂ށj��\���܂��B
			D3D_DRIVER_TYPE_HARDWARE, // D3D_DRIVER_TYPE �h���C�o�[�̎�ނ̃I�v�V����
			NULL, // Software �\�t�g�E�F�A���X�^���C�U�[����������DLL�ւ̃n���h��
			0, // Flags �l��L���ɂ��邽�߂̃����^�C�����C���[�́A�r�b�g�P�ʂ�OR���ꏏ�ɂ��邱�Ƃ��ł��܂��B
			featureLevelArray, // �쐬���悤�Ƃ���@�\���x���̏��������肷��D3D_FEATURE_LEVEL�̔z��ւ̃|�C���^�[�B
			2, // pFeatureLevels�̗v�f��
			D3D11_SDK_VERSION, // SDK�o�[�W�����BD3D11_SDK_VERSION���g�p����B
			device.GetAddressOf(), // �쐬�����f�o�C�X�̃I�u�W�F�N�g�ւ̃|�C���^�[�̃A�h���X��Ԃ�
			&featureLevel,	//�@���������ꍇ�A��������pFeatureLevels�z�񂩂�ŏ���D3D_FEATURE_LEVEL��Ԃ��܂��B 
					//�@�ǂ̋@�\���x�����T�|�[�g����Ă��邩�𔻒f����K�v���Ȃ��ꍇ�́A���͂Ƃ���NULL���w�肵�܂��B
			context.GetAddressOf() //�@�쐬�����f�o�C�X�R���e�L�X�g�I�u�W�F�N�g�ւ̃|�C���^�[�̃A�h���X��Ԃ�
		)))
		{
			MessageBoxW(hWnd, L"�f�o�C�X�̍쐬�Ɏ��s(beginning_System)", L"ERR", MB_ICONSTOP);
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



		//�C���^�[�t�F�[�X�擾
		Microsoft::WRL::ComPtr <IDXGIDevice> dxgi = nullptr;
		// https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(refiid_void)
		hr = device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgi.GetAddressOf()));
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))	return hr;

		//�A�_�v�^�[�擾
		Microsoft::WRL::ComPtr <IDXGIAdapter> adapter;
		// https://docs.microsoft.com/ja-jp/windows/win32/api/dxgi/nf-dxgi-idxgidevice-getadapter
		hr = dxgi->GetAdapter(adapter.GetAddressOf());
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))	return hr;

		////�t�@�N�g���[�擾
		Microsoft::WRL::ComPtr <IDXGIFactory> factory;
		// https://docs.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-getparent
		// �w�肵���E�B���h�E�̐e�܂��͏��L�҂ւ̃n���h�����擾���܂��B
		hr = adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf()));
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));
		if (FAILED(hr))	return hr;



		// DXGI_SWAP_CHAIN_DESC SwapChain�\����
		DXGI_SWAP_CHAIN_DESC swapChain_Desc;
		// �𑜓x�̏c���Ɖ�����ݒ肷��
		swapChain_Desc.BufferDesc.Width = static_cast<UINT>(screenWidth);
		swapChain_Desc.BufferDesc.Height = static_cast<UINT>(screenHeight);
		// ���t���b�V�����[�g���w���c�P�ʂŋL�q
		swapChain_Desc.BufferDesc.RefreshRate.Numerator = 0;
		swapChain_Desc.BufferDesc.RefreshRate.Denominator = 0;
		// �\���t�H�[�}�b�g���L�q
		// �A���t�@���܂ރ`���l�����Ƃ�8�r�b�g���T�|�[�g����4�R���|�[�l���g��32�r�b�g�����Ȃ����K������sRGB�`���B
		swapChain_Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// �X�L�������C���`�惂�[�h���L�q����񋓌^ 
		// DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED �w��Ȃ�
		swapChain_Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		// �X�P�[�����O���[�h���L�q����񋓌^
		// DXGI_MODE_SCALING_UNSPECIFIED �w��Ȃ�
		swapChain_Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		//�@�}���`�T���v�����O�p�����[�^���L�q���� DXGI_SAMPLE_DESC �\���́B
#if MSAA
		swapChain_Desc.SampleDesc = sampleDesc; // MSAA

#elif !MSAA 
		swapChain_Desc.SampleDesc.Count = 1;  // �P�s�N�Z��������̃}���`�T���v�����B
		swapChain_Desc.SampleDesc.Quality = 0; //�掿���x���̂��ƁB�掿�������قǃp�t�H�[�}���X���ቺ���܂��B�L���Ȕ͈͂�0�`�P
#endif 
	// DXGI_USAGE �񋓌^�̃����o�ŁA�o�b�N �o�b�t�@�̃T�[�t�F�C�X�g�p�ʂ� CPU �A�N�Z�X �I�v�V�������L�q���܂��B
	// �o�b�N�o�b�t�@�́A�V�F�[�_���͂܂��̓����_�[�^�[�Q�b�g�o�͂Ɏg�p���邱�Ƃ��ł��܂��B
		swapChain_Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �T�[�t�F�X�܂��̓��\�[�X���o�̓����_�[�^�[�Q�b�g�Ƃ��Ďg�p���܂��B
		// �X���b�v�`�F�[�����̃o�b�t�@�̐���\���l�B
		swapChain_Desc.BufferCount = 1;
		// �o�̓E�B���h�E�ւ�HWND�n���h���B ���̃����o�[��NULL�ł����Ă͂Ȃ�܂���B
		swapChain_Desc.OutputWindow = hWnd;
		// �o�͂��E�B���h�E���[�h�ł��邩�ǂ������w�肷��u�[���l �o�͂��E�B���h�E���[�h�̏ꍇ��TRUE ����ȊO�̏ꍇ�AFALSE
		swapChain_Desc.Windowed = TRUE;
		// �T�[�t�F�X�񎦌�̃v���[���e�[�V�����o�b�t�@�̓��e����������I�v�V�������L�q���� DXGI_SWAP_EFFECT �񋓌^�̃����o
		swapChain_Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		// DXGI_SWAP_CHAIN_FLAG �񋓌^�̃����o�ŁA�X���b�v�`�F�[������̃I�v�V�������L�q����
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
		// DepthStencilView�̍쐬
		// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_texture2d_desc
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = static_cast<UINT>(screenWidth);
		textureDesc.Height = static_cast<UINT>(screenHeight);
		// �e�N�X�`�����̃~�b�v�}�b�v���x���̍ő吔
		// �~�b�v�}�b�v�̐����@https://yttm-work.jp/gmpg/gmpg_0018.html
		textureDesc.MipLevels = 1;
		// �e�N�X�`���z����̃e�N�X�`���̐�
		textureDesc.ArraySize = 1;
		// HACK: �[�x�X�e���V���Ƃ��ė��p�ł�����̂͂S�ł킯���킩��Ȃ��̂ňႢ������i�K�Ő؂�ւ���
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
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_usage ��ʓI�Ɏg����̂�D3D11_USAGE_DEFAULT
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		// �p�C�v���C���X�e�[�W�Ƀo�C���h���邽�߂̃t���O
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_bind_flag
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		// �������CPU�A�N�Z�X�̃^�C�v���w�肷��t���O CPU�A�N�Z�X���s�v�ȏꍇ��0���g�p
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_cpu_access_flag
		textureDesc.CPUAccessFlags = 0;
		// ���܂��ʓI�ł͂Ȃ����̃��\�[�X�I�v�V���������ʂ���t���O
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_resource_misc_flag
		textureDesc.MiscFlags = 0;
		// CreateTexture2D
		// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createtexture2d
		if (FAILED(hr = device->CreateTexture2D(&textureDesc, NULL, texture2D.GetAddressOf())))
		{
			MessageBoxW(hWnd, L"�[�x�X�e���V���o�b�t�@�̍쐬�Ɏ��sbeginning_System)", L"ERR", MB_ICONSTOP);
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsDESC = {};
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_depth_stencil_view_desc
		dsDESC.Format = textureDesc.Format;
		// ���\�[�X�̃^�C�v �[�x�X�e���V�����\�[�X�ւ̃A�N�Z�X���@���w�肷�� �l�͂��̍\���̂̋��p�̂Ɋi�[�����
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_dsv_dimension
		dsDESC.ViewDimension = textureDesc.SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		// �g�p����ŏ��̃~�b�v�}�b�v���x���̃C���f�b�N�X
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_tex2d_dsv
		dsDESC.Texture2D.MipSlice = 0;
		dsDESC.Flags = 0;
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/nf-d3d11-id3d11device-createdepthstencilview
		if (FAILED(hr = device->CreateDepthStencilView(texture2D.Get(), &dsDESC, depthStencilView.GetAddressOf())))
		{
			MessageBoxW(hWnd, L"�[�x�X�e���V���r���[�̍쐬�Ɏ��s(beginning_System)", L"ERR", MB_ICONSTOP);
			return false;
		}

		return true;
	}


	bool DirectX11Manager::InitializeRenderTarget(const HWND hWnd)
	{

		HRESULT hr = S_OK;
		if (FAILED(hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), static_cast<void**>(&texture2D))))
		{
			MessageBoxW(hWnd, L"�o�b�N�o�b�t�@�̎擾�Ɏ��s(beginning_System)", L"ERR", MB_ICONSTOP);
			return false;
		}
		// �����_�[�^�[�Q�b�g�r���[�̍쐬
		// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createrendertargetview
		if (FAILED(hr = device->CreateRenderTargetView(texture2D.Get(), NULL, renderTargetView.GetAddressOf())))
		{
			MessageBoxW(hWnd, L"�����_�[�^�[�Q�b�g�r���[�̍쐬�Ɏ��s(beginning_System)", L"ERR", MB_ICONSTOP);
			return false;
		}

		CreateDepthStencil(hWnd);

		context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		SetViewPort(screenWidth, screenHeight);

		return true;
	}


	void DirectX11Manager::SetViewPort(const float width, const float height)
	{
		// �r���[�|�[�g�̐ݒ�
		// https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_viewport
		D3D11_VIEWPORT Viewport;
		// �r���[�|�[�g�̕�
		Viewport.Width = width;
		// �r���[�|�[�g�̍���
		Viewport.Height = height;
		// �r���[�|�[�g�̍ŏ��[�x
		Viewport.MinDepth = 0.0f;
		// �r���[�|�[�g�̍ő�[�x
		Viewport.MaxDepth = 1.0f;
		// �r���[�|�[�g�̍�����X�ʒu �܂�O
		Viewport.TopLeftX = 0.0f;
		// �r���[�|�[�g�̏㕔��Y�ʒu �܂�O
		Viewport.TopLeftY = 0.0f;
		context->RSSetViewports(1, &Viewport);
	}

}

