#pragma once

#include <vector>
#include <unordered_map>
#include "Component.h"

class Game;	// circular dependencies


class System
{
public:
	System();
	~System();
	virtual void update(float dt) = 0;

protected:
	virtual void onComponentCreated(std::type_index t, Component* c) = 0;
	virtual void onComponentDestroyed(std::type_index t, Component* c) = 0;
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