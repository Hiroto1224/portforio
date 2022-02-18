#pragma once

#include <Windows.h>

#include "ExistObject.h"

class ExistObject;


class DebugUI
{
public:
	DebugUI() = default;
	virtual ~DebugUI() = default;

	// Starting Over���Ŏg���֐�
	void Initialize(HWND hWnd);
	void GameLoop();
	void Render();
	void UnInitialize();
	bool WinProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
private:
	void ImGuiLoadJapaneseFont();

	std::shared_ptr<ExistObject> selectObject;

public:

	// ���i�g������֐�
	void SelectCreateObject();
	void DebugUpdate();
	void Inspector();
	void Hierarchy();
};