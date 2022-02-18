#pragma once
#include <memory>
#include "Scene.h"

class SceneManager
{
public:
	/// <summary>
	/// ���݂�scene
	/// </summary>
	/// <returns></returns>
	static std::unique_ptr<Scene>& GetActiveScene();

	static void SaveScene();
	static void LoadScene();

private:
	static std::unique_ptr<Scene> activeScene;


};