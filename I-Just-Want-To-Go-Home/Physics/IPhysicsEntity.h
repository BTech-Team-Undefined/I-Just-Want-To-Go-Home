#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Point.h"

using namespace std;

class IPhysicsEntity
{
public:
	int _physicsId = -1;
	string _physicsName = "";
	Point _roughCenter = Point(0, 0);
	float _roughRadius = 0;
	vector<Point> _collider;
	vector<int> _collidingIds;

	IPhysicsEntity()
	{
	}

	virtual ~IPhysicsEntity()
	{
	}

	virtual void OnCollision(int entityId, string entityName)
	{
		cout << "Collision occured: " + this->_physicsName + "(id: " + to_string(this->_physicsId) + ") & " + entityName + "(id: " + to_string(entityId) + ")" << endl;
	}

	virtual void SetPhysicsInfo(int id, string name)
	{
		this->_physicsId = id;
		this->_physicsName = name;
	}

	virtual void SetCollider(vector<Point> vertices, Point roughCenter, float roughRadius)
	{
		
		this->_collider.clear();
		this->_collider.insert(this->_collider.end(), vertices.begin(), vertices.end());
		this->_roughCenter = roughCenter;
		this->_roughRadius = roughRadius;
	}
};

