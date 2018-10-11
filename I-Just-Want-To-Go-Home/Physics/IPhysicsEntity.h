#pragma once

#include <iostream>

using namespace std;

class IPhysicsEntity
{
public:
	IPhysicsEntity()
	{
	}

	virtual ~IPhysicsEntity()
	{
	}

	virtual void OnCollision(int entityId, string entityName)
	{
	}
};

