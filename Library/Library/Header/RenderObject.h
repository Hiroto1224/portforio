#pragma once
#include "MyMath.h"
#include "Shader.h"
#include "Component.h"
#include "ExistObject.h"
#include <cereal/types/polymorphic.hpp>
class Component;

class RenderObject : public Component
{
public:
	~RenderObject() override = default;
	//virtual void Initialize(const std::shared_ptr<ExistObject>& object) {};

	virtual void Render(){};

	virtual void RenderShadow(){};

	bool IsActive() const { return !fileName.empty(); }


protected:
	std::string fileName;

	virtual void Load(){};

public:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Component>(this),fileName);
	}
};

CEREAL_REGISTER_TYPE(RenderObject)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, RenderObject)
