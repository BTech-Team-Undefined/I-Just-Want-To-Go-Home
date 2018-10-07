#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <typeinfo>
#include <typeindex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Component.h"

class Entity
{
public:
	glm::vec3 position;
	glm::vec3 rotation; 
	glm::vec3 scale = glm::vec3(1.0f); 
	
	//Adds a component 
	template<class T>
	void AddComponent() { _components[typeid(T)] = std::shared_ptr<T>(new T(this)); }
	
	//todo make components in isolation set Entity later in add.
	template<class T>
	std::shared_ptr<T> GetComponent() { return std::static_pointer_cast<T>(_components[typeid(T)]); }
	
	template<class T>
	void RemoveComponent() 
	{
		if (GetComponent<T>() != nullptr)
		{
			GetComponent<T>()->Kill();
			_components.erase(typeid(T));
		}
	}

	Entity* getParent() { return _parent; }
	void setParent(Entity* entity) { _parent = entity; }

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

private:
	unsigned int id = 0;
	std::unordered_map <std::type_index, std::shared_ptr<Component>> _components;
	std::vector<std::unique_ptr<Entity>> _children;
	Entity* _parent;
	// may want to store a private world position,rotation,scale which can optimize code 
};