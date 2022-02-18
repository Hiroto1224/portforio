#pragma once
#include <string>

#include "../../myLibScripts/Header/ExistObject.h"
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "../../myLibScripts/Header/RigidBody.h"

// interface
class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void Render() = 0;

	virtual std::shared_ptr<ExistObject> Find(const std::string& name) = 0;
	virtual std::shared_ptr<ExistObject> CreateObject() = 0;
	virtual std::map<std::string/* object type */, std::vector<std::shared_ptr<ExistObject>>> ObjectList() = 0;
	virtual	void AddRigidBody(std::shared_ptr<RigidBody> btRigidBody) = 0;
	virtual void save() = 0;
	virtual void load() = 0;

};


