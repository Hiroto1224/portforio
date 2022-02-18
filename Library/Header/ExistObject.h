#pragma once
#include "Object.h"
#include <memory>


class ExistObject : public Object
{
public:
	
	static std::shared_ptr<ExistObject> Find(const std::string& findName);


	std::shared_ptr<Transform> GetTransform() const;

	std::vector<std::shared_ptr<Component>> GetComponents() const;

	void SetTransform(const std::shared_ptr<Transform> setTransform);

	template<class T>
	std::shared_ptr<T> AddComponent();

	void Initialize();

	void Update();

	void CreateObjectTemplate(const std::string& fileName);

	void LoadObjectTemplate(const std::string& fileName);

	void ClearComponents();
protected:


	std::shared_ptr<Transform> transform;

	std::vector<std::shared_ptr<Component>> components;
private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive);
};
CEREAL_REGISTER_TYPE(ExistObject)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, ExistObject)

template<class T>
std::shared_ptr<T> ExistObject::AddComponent()
{
	const std::shared_ptr<T> component = std::make_shared<T>();
	std::dynamic_pointer_cast<Component>(component)->Initialize(std::static_pointer_cast<ExistObject>(shared_from_this()));
	components.push_back(component);


	return nullptr;

}

