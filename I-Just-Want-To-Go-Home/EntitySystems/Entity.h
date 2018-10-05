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
private:
	unsigned int id = 0;
	std::unordered_map <std::type_index, std::shared_ptr<Component>> _components;
	std::vector<std::unique_ptr<Entity>> _children;
};