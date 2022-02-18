#include "../Header/RenderObjectManager.h"
#include "../Header/RenderObject.h"
#include "../Header/ExistObject.h"
#include "../Header/StaticMesh.h"
#include "../Header/SkinnedMesh.h"

std::vector<std::weak_ptr<RenderObject>> RenderObjectManager::renderers;


void RenderObjectManager::Initialize()
{
	renderers.clear();

}

void RenderObjectManager::Render()
{
	for(const auto& renderObject : renderers)
	{
		if(!renderObject.lock())
		{
			continue;
		}
		if (!renderObject.lock()->IsActive()) continue;

		renderObject.lock()->Render();
	}
}

void RenderObjectManager::RenderShadow()
{
	for (const auto& renderObject : renderers)
	{
		if (!renderObject.lock())
		{
			continue;
		}
		if (!renderObject.lock()->IsActive()) continue;

		renderObject.lock()->RenderShadow();
	}
}

void RenderObjectManager::AddRenderObject(std::shared_ptr<StaticMesh> object)
{

	renderers.push_back(object);
}

void RenderObjectManager::AddRenderObject(std::shared_ptr<SkinnedMesh> object)
{

	renderers.push_back(object);
}
