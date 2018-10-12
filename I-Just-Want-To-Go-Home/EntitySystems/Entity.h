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
	Entity() :_id(0) {}
	Entity(Entity* parent) :_id(0)
	{
		setParent(parent);
	}

	void update(float dt)
	{
		for (const auto& c : _components)
			c.second->update(dt);
		for (const auto& c : _children)
			c->update(dt);
	}

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
			_children.push_back(child);
			child->setParent(this);
		}
	}
	//returns child with id
	Entity* getChild(unsigned int id) const { return *std::find_if(_children.begin(), _children.end(), [&id](const Entity* e) { return e->getID() == id; }); }
	std::vector<Entity*>& getChildren() { return _children; }
	//removes child with id
	void removeChild(unsigned int id) 
	{
		auto t = std::find_if(_children.begin(), _children.end(), [&id](const Entity* e) { return e->getID() == id; });
		_children.erase(t);
		(*t)->setParent(nullptr);
	}

private:
	unsigned int _id = 0;//todo add bullshit with id generation
	bool _enabled = true;

	glm::vec3 _position;
	glm::vec3 _rotation;
	glm::vec3 _scale;

	glm::vec3 _worldPosition;
	glm::vec3 _worldRotation;
	glm::vec3 _worldScale;

	std::unordered_map <std::type_index, std::shared_ptr<Component>> _components;
	std::vector<Entity*> _children;
	Entity* _parent;
};