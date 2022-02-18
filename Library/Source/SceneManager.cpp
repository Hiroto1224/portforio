#include "../Header/SceneManager.h"
#include "../../HideSeek/Header/GameScene.h"

std::unique_ptr<Scene> SceneManager::activeScene = std::make_unique<GameScene>();


std::unique_ptr<Scene>& SceneManager::GetActiveScene()
{
	return activeScene;
}

void SceneManager::SaveScene()
{
	activeScene->save();

}
void SceneManager::LoadScene()
{
	activeScene->load();

}