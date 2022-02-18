#pragma once
#include <Windows.h>
#include <sstream>
#include <memory>

#include "high_resolution_timer.h"
#include "CameraManager.h"
#include "Blender.h"
#include "DebugUI.h"
#include "SceneManager.h"

namespace StartingOver
{
	//-------------------------------------
	// ���C�u���������ݒ�
	//-------------------------------------
	// LPCSTR caption		�^�C�g���o�[�̌��o��
	// float width			�E�B���h�E�̕�
	// float height			�E�B���h�E�̍���
	// bool isFullscreen	�t���X�N���[���ɂ��邩�ǂ���
	// double frameRate		�t���[�����[�g
	void Initialize(LPCTSTR, float width = (640.0f), float height = (480.0f),
		bool isFullscreen = (false), double frameRate = (144.0f));

	//--------------------------------------
	// ���C�u�����̏I������
	//--------------------------------------
	void UnInitialize();

	//--------------------------------------
	//�Q�[�����[�v
	//--------------------------------------
	bool GameLoop(bool isFrameRate);


	//--------------------------------------
	//��ʃN���A
	//--------------------------------------
	// float r				�h��Ԃ��F (r) 
	// float g				�h��Ԃ��F (g) 
	// float a				�h��Ԃ��F (b) 
	//--------------------------------------
	void ClearDisp(float r = (1.0f), float g = (1.0f), float b = (1.0f));

	//--------------------------------------
	//���̃o�b�N�o�b�t�@�̓��e��\��
	//--------------------------------------
	HRESULT Present(UINT syncInterval, UINT flags);

	void SetBlendMode(Blender::BLEND_STATE);

	namespace Window
	{
		static HWND hWnd;

		HWND WindowInitialize(LPCTSTR windowName, float width, float height);

		void WindowUnInitialize();
	}

	class GameManager
	{
	public:
		static std::unique_ptr<DebugUI> debugUi;
		static std::unique_ptr<SceneManager> sceneManager;

	};

	//--------------------------------
   //  FPS�v�Z
   //--------------------------------
	static void CalculateFrameStats(const HWND hWnd, const HighResolutionTimer* timer)
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.
		static int frames = 0;
		static float timeElapsed = 0.0f;

		frames++;

		// Compute averages over one second period.
		if ((timer->timeStamp() - timeElapsed) >= 1.0f)
		{
			const auto fps = static_cast<float>(frames); // fps = frameCnt / 1
			const float mspf = 1000.0f / fps;
			std::ostringstream outs;
			outs.precision(6);
			outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
			SetWindowTextA(hWnd, outs.str().c_str());

			// Reset for next average.
			frames = 0;
			timeElapsed += 1.0f;
		}
	}


}