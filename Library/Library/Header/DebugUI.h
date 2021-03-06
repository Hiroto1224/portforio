#pragma once

#include <Windows.h>

#include "ExistObject.h"

class ExistObject;


class DebugUI
{
public:
	DebugUI() = default;
	virtual ~DebugUI() = default;

	// Starting Over内で使う関数
	void Initialize(HWND hWnd);
	void GameLoop();
	void Render();
	void UnInitialize();
	bool WinProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
private:
	void ImGuiLoadJapaneseFont();

	std::shared_ptr<ExistObject> selectObject;

public:

	// 普段使いする関数
	void SelectCreateObject();
	void DebugUpdate();
	void Inspector();
	void Hierarchy();
};