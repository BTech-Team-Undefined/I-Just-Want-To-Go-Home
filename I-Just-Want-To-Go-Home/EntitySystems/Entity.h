#pragma once

#define GLM_ENABLE_EXPERIMENTAL	// I have no idea why we can't put this in main

#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <typeinfo>
#include <typeindex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "Component.h"

class Entity
{
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale = glm::vec3(1.0f);

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
	void getComponents(std::vector<std::shared_ptr<T>>& results)
	{
		// add component on this entity 
		auto comp = getComponent<T>();
		if (comp) results.push_back(comp);
		// add components on children 
		for (int i = 0; i < getChildren().size(); i++)
		{
			getChildren()[i]->getComponents<T>(results);
		}
	}

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
		_parent = parent;
		_parent->addChild(this);
	}

	Entity* getParent() { return _parent; }
	void addChild(Entity* child)
	{
		child->_parent = this;
		_children.push_back(child);
	}

	//returns child with id
	Entity* getChild(unsigned int id) { return *std::find_if(_children.begin(), _children.end(), [id](Entity* e) { return e->getID() == id; }); }

	//removes child with id
	void removeChild(unsigned int id)
	{
		auto t = std::find_if(_children.begin(), _children.end(), [&id](const Entity* e) { return e->getID() == id; });
		_children.erase(t);
		(*t)->setParent(nullptr);
	}

	glm::mat4 getLocalTransformation()
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
		model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
		model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
		model = glm::scale(model, scale);
		return model;
	}

	glm::mat4 getWorldTransformation()
	{
		glm::mat4 transform = getLocalTransformation();
		if (getParent() == nullptr)
			return transform;
		else
			return transform * getParent()->getWorldTransformation();
	}

	void setLocalTransform(glm::mat4 matrix)
	{
		// warning: assuming matrix bottom-right value is 1. If not divide everything in matrix by that value. 
		glm::quat rot; 
		glm::vec3 skew;
		glm::vec4 perspective; 
		glm::decompose(matrix, scale, rot, position, skew, perspective);
		rotation = glm::eulerAngles(rot);
	}

	std::vector<Entity*> const& getChildren() const { return _children; }

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
	// may want to store world position,scale,rotation for optimization
};



#pragma once



