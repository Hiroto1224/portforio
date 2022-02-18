#pragma once
#include <string>
#include <memory>
#include "MyMath.h"

#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/atomic.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/functional.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/forward_list.hpp>


class Transform;
class ExistObject;
class Component;

class Object : public std::enable_shared_from_this<Object>
{
public:
	Object() = default;
	virtual ~Object() = default;

	std::string GetName() const;
	void SetName(const std::string& setName);

	std::string GetTag() const;
	void SetTag(const std::string& setTag);


protected:
	// objectの名前
	std::string name;
	// ヒエラルキーの順番整理のため
	std::string tag;

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive);
	
};
CEREAL_REGISTER_TYPE(Object)