#pragma once
#include "Transform.h"
#include "../Header/MyMath.h"


// agent�̐����Ɣj����S���@�\��
class AgentManager
{
public:
	AgentManager() = default;
	virtual ~AgentManager() = default;

	void Initialize();

	void Update();

	void DebugUI();


	struct CreateData
	{
		int agentTypeNumber = -1;
		Transform createTransform;

	};

	void CreateAgent(CreateData createData) const;

	
private:
	

public:
	

private:
	CreateData createData;
};
