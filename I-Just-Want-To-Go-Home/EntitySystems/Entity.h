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

	Entity();

	Entity(Entity* parent);

	void update(float dt);

	unsigned int getID() const;

	//Adds a component 
	template<class T>
	void addComponent()
	{
		_components[typeid(T)] = std::shared_ptr<T>(new T(this));
	}

	//todo make components in isolation set Entity later in add.
	template<class T>
	std::shared_ptr<T> getComponent()
	{
		return std::static_pointer_cast<T>(_components[typeid(T)]);
	}

	template<class T>
	void getComponents(std::vector<std::shared_ptr<T>>& results)
	{
		// add component on this entity 
		auto comp = getComponent<T>();
		if (comp)
			results.push_back(comp);
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

	void setParent(Entity* parent);

	Entity* getParent();
	
	void addChild(Entity* child);

	//returns child with id
	Entity* getChild(unsigned int id);

	//removes child with id
	void removeChild(unsigned int id);

	glm::mat4 getLocalTransformation();

	glm::mat4 getWorldTransformation();

	void setLocalTransform(glm::mat4 matrix);

	std::vector<Entity*> const& getChildren() const;

private:
	unsigned int _id = 0;
	static unsigned int _curID;
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