#include "../Header/Object.h"
#include "../Header/ExistObject.h"
#include "../Header/Transform.h"
#include "../Header/Component.h"
using namespace std;

std::string Object::GetName() const
{
	return name;
}

void Object::SetName(const std::string& setName)
{
	name = setName;
}

std::string Object::GetTag() const
{
	return tag;
}

void Object::SetTag(const std::string& setTag)
{
	tag = setTag;
}

template<class Archive>
void Object::serialize(Archive& archive)
{
	archive(name, tag);
}