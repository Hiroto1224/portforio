#pragma once
#include <vector>
#include <memory>

class RenderObject;
class ExistObject;
class StaticMesh;
class SkinnedMesh;

class RenderObjectManager
{
public:
	RenderObjectManager() = default;
	virtual ~RenderObjectManager() = default;

	static void Initialize();

	static void Render();
	static void RenderShadow();

	static void AddRenderObject(std::shared_ptr<StaticMesh> object);
	static void AddRenderObject(std::shared_ptr<SkinnedMesh> object);

private:

	static std::vector<std::weak_ptr<RenderObject>> renderers;

};

