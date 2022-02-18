#pragma once
#include <memory>

#include "../../myLibScripts/Header/FrameBufferHandler.h"
#include "../../myLibScripts/Header/SamplerStateHandler.h"
#include "../../myLibScripts/Header/RasterizerHandler.h"
#include "../../myLibScripts/Header/DepthStencilStateHandler.h"
#include "../../myLibScripts/Header/RendererConstatnts.h"
#include "../../myLibScripts/Header/Light.h"
#include "../../myLibScripts/Header/Camera.h"
#include "../../myLibScripts/Header/SkyMap.h"
#include "../../myLibScripts/Header/ExistObject.h"
#include "../../myLibScripts/Header/Scene.h"
#include "../../myLibScripts/Header/AgentManager.h"
#include "../../myLibScripts/Header/ObjectManager.h"
#include "../../myLibScripts/Header/BulletPhysics.h"


class GameScene : public Scene
{
public:
	GameScene() = default;
	~GameScene() override = default;
	GameScene(GameScene&) = delete;
	GameScene& operator=(GameScene&) = delete;

	void Initialize() override;

	void Update() override;

	void Render() override;

	std::shared_ptr<ExistObject> Find(const std::string& name) override;
	std::shared_ptr<ExistObject> CreateObject() override;
	std::map<std::string/* object type */, std::vector<std::shared_ptr<ExistObject>>> ObjectList();
	void AddRigidBody(std::shared_ptr<RigidBody> btRigidBody) override;
	void save()override;
	void load()override;

private:
	// ÉnÉìÉhÉâÅ[
	std::unique_ptr<FrameBufferHandler> frameBufferHandler;
	std::unique_ptr<SamplerStateHandler> samplerStateHandler;
	std::unique_ptr<RasterizerHandler> rasterizerHandler;
	std::unique_ptr<DepthStencilStateHandler> depthStencilStateHandler;

	std::unique_ptr<RendererConstants> rendererConstants;

	std::unique_ptr<Light> light;
	std::shared_ptr<Camera> sceneCamera;
	std::shared_ptr<ExistObject> cameraObject;
	std::shared_ptr<SkyMap> skyMap;
	std::shared_ptr<ExistObject> skyMapObject;
	std::unique_ptr<AgentManager> agentManager;
	std::unique_ptr<ObjectManager> objectManager;

	std::map<std::string/* object type */, std::vector<std::shared_ptr<ExistObject>>> objectList;

	std::unique_ptr<BulletPhysics> bulletPhysics;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(objectList);
	}

};
