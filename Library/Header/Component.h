#pragma once

#include "Object.h"

#include <string>
#include <memory>

class Component : public Object
{
public: // function
	virtual void Initialize(const std::shared_ptr<ExistObject> existObject){};

	/// <summary>
	/// inspectorに変更可能なパラメーターをImGuiで表示する
	/// </summary>
	virtual void DebugUI(){};

	virtual void Update() {};
	std::weak_ptr<ExistObject> existObject;

	std::weak_ptr<Transform> transform;

public: // variable

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive);

};
CEREAL_REGISTER_TYPE(Component)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, Component)

template<class Archive>
void Component::serialize(Archive& archive)
{
	archive(cereal::base_class<Object>(this));
}