#include "../Header/ExistObject.h"
#include "../Header/Component.h"
#include "../Header/Transform.h"

#include <fstream>
#include "../Header/SceneManager.h"
#include "../../HideSeek/Header/Block.h"
#include <memory>


void ExistObject::Initialize()
{
	for(const auto& component : components)
	{
		component->Initialize(std::static_pointer_cast<ExistObject>(shared_from_this()));
	}
}

void ExistObject::Update()
{
	for (const auto& component : components)
	{
		component->Update();
	}
}

std::shared_ptr<ExistObject> ExistObject::Find(const std::string& findName)
{
	// Œ»Ý‚Ìscene“à‚É‘¶Ý‚µ‚Ä‚¢‚éObject‚ðŒŸõ
	// “¯‚¶–¼‘O‚ð‚Â‚¯‚Ä‚¢‚é‚Æ‚¤‚Ü‚­ŒŸõ‚Å‚«‚È‚¢‚Ì‚Å“¯‚¶–¼‘O‚Í‚Â‚¯‚È‚¢
	return  SceneManager::GetActiveScene()->Find(findName);
}

std::shared_ptr<Transform> ExistObject::GetTransform() const
{
	return transform;
}

std::vector<std::shared_ptr<Component>> ExistObject::GetComponents() const
{
	return components;
}

void ExistObject::SetTransform(const std::shared_ptr<Transform> setTransform)
{
	transform = setTransform;
}

void ExistObject::ClearComponents()
{
	for(auto& component : components)
	{
		component.reset();
	}
	components.clear();
}


void ExistObject::CreateObjectTemplate(const std::string& fileName)
{
	std::ofstream stream;
	stream.open(std::string("Data/ObjectTemplate/"+fileName).c_str(), std::ios::binary);
	cereal::BinaryOutputArchive archive(stream);
	archive(cereal::base_class<Object>(this), transform, components);
}

void ExistObject::LoadObjectTemplate(const std::string& fileName)
{
	std::ifstream stream;
	stream.open(fileName.c_str(), std::ios::binary);
	cereal::BinaryInputArchive archive(stream);
	archive(cereal::base_class<Object>(this), transform, components);

	Initialize();
}


template<class Archive>
void ExistObject::serialize(Archive& archive)
{
	archive(cereal::base_class<Object>(this), transform, components);
}