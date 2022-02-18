#pragma once
#include "Transform.h"
#include "../Header/MyMath.h"

// object‚Ì¶¬‚Æ”jŠü‚ğ’S“–@—\’è
class ObjectManager
{
public:
	ObjectManager() = default;
	virtual ~ObjectManager() = default;

	void Initialize();

	void Update();

	void DebugUI();


	struct CreateData
	{
		int objectTypeNumber = -1;
		Transform createTransform;

	};

	void CreateAgent(CreateData createData);


private:


public:


private:
	std::vector<std::weak_ptr<ExistObject>> objectPool;

	CreateData createData;


	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(objectPool);
	}
};
