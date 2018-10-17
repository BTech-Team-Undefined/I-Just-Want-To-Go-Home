#pragma once

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include "Component.h"

class Game;	// circular dependencies


class System
{
	typedef std::unordered_map<std::type_index, std::vector<Component*>> ComponentMap;
public:
	System();
	~System();
	virtual void update(float dt) = 0;
	// TODO: Fix
	virtual void addComponent(std::type_index t, Component* component)
	{
		auto type = std::type_index(typeid(component));
		_componentMap[type].push_back(component);
	}
	virtual void clearComponents()
	{
		for (ComponentMap::iterator it = _componentMap.begin(); it != _componentMap.end(); ++it)
		{
			it->second.resize(0);
		}
	}

protected:
	virtual void onComponentCreated(std::type_index t, Component* c) = 0;
	virtual void onComponentDestroyed(std::type_index t, Component* c) = 0;
	ComponentMap _componentMap;
};


/* Tentative: 

class System 
{
	map < T , ComponentList<T> > 

	void addListener<T>
	{
		map[ t ] = new ComponentList < T> 
	}

	vector<T> getComponents<T>
	{
		return map [ t] . getList()
	}
}

class ComponentList < T >
{
	vector<T> list;

	ctor
	{
		Game::instance()->addlistener()
	}

	dtor
	{
		Game::instance()->removelistener()
	}

	void onComponentCreated(Component)
	{
		if (typeid(component) == typeid(T))
			list.add( cast <T> component);
	}

	void onComponentDestroyed()
	{
		if (typeid(component) == typeid(T))
			list.remove( component );
	}

	vector<T> * getList() 
	{
		return list; 
	}

	iterator<T> getValidComponents()
	{
		iterate thru list 
			ensure component is active
			ensure parent exists
			ensure entity is active (not dangling too)
			return value
	}
}

class derived system 
{
	ctor 
	{
		addlistener < T1 > ()
		addlistener < T2 > ()
		addlistener < T3 > ()
	}

	update 
	{
		foreach T1 in getComponents <T1>
			// ...
		foreach T2 in getComponents <T2>
			// ...
		foreach T3 in getComponents <T3>
			// ...
	}
}
*/