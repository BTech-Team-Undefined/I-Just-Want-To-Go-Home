#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <typeinfo>
#include <typeindex>
#include "Component.h"

class Entity
{
public:
	template<class T>
	void AddComponent() { _components[typeid(T)] = std::shared_ptr<T>(new T()); }
	template<class T>
	std::shared_ptr<T> GetComponent() { return std::static_pointer_cast<T>( _components[typeid(T)] ); }
	template<class T>
	void RemoveComponent() { GetComponent<T>()->Kill(); _components.erase(typeid(T)); }
private:
	unsigned int id;
	std::unordered_map <std::type_index, std::shared_ptr<Component>> _components;
	std::vector<std::unique_ptr<Entity>> _children;
};