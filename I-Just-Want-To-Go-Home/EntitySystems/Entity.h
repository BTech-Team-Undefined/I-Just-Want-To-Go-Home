#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <typeinfo>
#include <typeindex>
#include <glm/glm.hpp>
#include "Component.h"

class Entity
{
public:
	glm::vec3 position;
	glm::vec3 rotation; 
	glm::vec3 scale; 
	
	unsigned int getID() const { return _id; }
	//Adds a component 
	template<class T>
	void addComponent() { _components[typeid(T)] = std::shared_ptr<T>(new T(this)); }
	//todo make components in isolation set Entity later in add.
	template<class T>
	std::shared_ptr<T> getComponent() { return std::static_pointer_cast<T>(_components[typeid(T)]); }
	template<class T>
	void removeComponent() 
	{
		if (getComponent<T>() != nullptr)
		{
			getComponent<T>()->Kill();
			_components.erase(typeid(T));
		}
	}
	void setParent(Entity* parent)
	{
		if (_parent != parent)
		{
			_parent = parent;
			_parent->addChild(this);
		}
	}
	Entity* getParent() const { return _parent; }
	void addChild(Entity* child)
	{
		if (std::find_if(_children.begin(), _children.end(), [child](const Entity* e) { return e->getID() == child->getID(); }) == _children.end())
		{
			child->setParent(this);
			_children.push_back(child);
		}
	}
	//returns child with id
	Entity* getChild(unsigned int id) const { return *std::find_if(_children.begin(), _children.end(), [&id](const Entity* e) { return e->getID() == id; }); }
	std::vector<Entity*>& getChildren() { return _children; }
	//removes child with id
	void removeChild(unsigned int id) 
	{
		auto t = std::find_if(_children.begin(), _children.end(), [&id](const Entity* e) { return e->getID() == id; });
		(*t)->setParent(nullptr);
		_children.erase(t);
	}

private:
	unsigned int _id = 0;
	std::unordered_map <std::type_index, std::shared_ptr<Component>> _components;
	std::vector<Entity*> _children;
	Entity* _parent;
};