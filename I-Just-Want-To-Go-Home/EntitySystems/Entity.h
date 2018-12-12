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

// TODO: Make everything unique_ptr - Entities own children and own components. 
class Entity
{
// Type definitions 
	typedef std::unordered_map <std::type_index, std::unique_ptr<Component>> ComponentMap;

// Variables 
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale = glm::vec3(1.0f);

private:
	static unsigned int _curID;
	unsigned int _id = 0;
	bool _enabled = true;
	bool _static = false;

	ComponentMap _components;					// component map 
	std::vector<Component*> _componentStorage;	// component storage
	std::vector<Entity*> _children;
	Entity* _parent;

	glm::vec3 _position;
	glm::vec3 _rotation;
	glm::vec3 _scale;

	glm::vec3 _worldPosition;
	glm::vec3 _worldRotation;
	glm::vec3 _worldScale;

	glm::mat4 _worldTransformation;

	// may want to store world position,scale,rotation for optimization

// Functions 
public: 
	Entity();
	Entity(Entity* parent);	
	Entity(unsigned int id);	// don't call this unless you know what you're doing.
	~Entity();

	// Returns entity's ID. This cannot change. 
	unsigned int getID() const;

	// Returns entity's enabled status. Does not check if parent is enabled.
	bool getEnabled() const;

	// Set entity's enabled status. Does not change children's status. 
	bool setEnabled(bool enabled);
	
	// DEPRECATED. Updates entity's components and children. Does not registers components to systems.
	void update(float dt);

	// Set the parent for this entity. Will properly bind entities together. 
	void setParent(Entity* parent);

	// Returns the parent entity. Can be null.
	Entity* getParent() const;
	
	// Adds entity as a child. Will properly bind entities together.
	void addChild(Entity* child);

	// Returns child entity with ID. 
	// Note: Will crash if not found.
	// TODO: Return nullptr if not found.
	Entity* getChild(unsigned int id);

	// Returns a reference to the list of child entities. 
	std::vector<Entity*> const& getChildren() const;

	// template definitions "must" be in header
	// https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl

	// Creates and adds a component to this entity. Component will be bound to this entity. 
	template<class T>
	void addComponent() {
		_components[typeid(T)] = std::make_unique<T>();
		_components[typeid(T)]->setEntity(this);
		_componentStorage.push_back(_components[typeid(T)].get());
	}

	// Returns a pointer to specified component. 
	template<class T>
	T* getComponent()
	{
		return static_cast<T*>(_components[typeid(T)].get());
	}

	// Returns a copy of all components attached to this entity. 
	const std::vector<Component*> getComponents()
	{
		return _componentStorage;
	}

	// Remove and destroy a component attached to this entity. 
	// WARNING: Removing a component on this entity during component update is undefined behaviour.
	// TOOD: Implement safe deletion of components. 
	template<class T>
	void removeComponent()
	{
		if (getComponent<T>() != nullptr)
		{
			getComponent<T>()->Kill();
			_componentStorage.erase(_components[typeid(T)].get());
			_components.erase(typeid(T));
		}
	}

	// Returns the local transformation matrix.
	glm::mat4 getLocalTransformation();

	// Returns the world transformation matrix (model matrix).
	glm::mat4 getWorldTransformation();
	
	// Returns the world position of the entity.
	glm::vec3 getWorldPosition();

	// Returns the world rotation of the entity.
	glm::vec3 getWorldRotation();

	// Returns the world scale of the entity;
	glm::vec3 getWorldScale();

	// Sets local transformation properties via a model matrix. 
	// Note: This uses experimental GLM functions and has edge cases. 
	void setLocalTransform(glm::mat4 matrix);

	// Sets local transformation properties via a world model matrix. 
	// Note: This uses experimental GLM functions and has edge cases. 
	// TODO: implement
	void setWorldTransform(glm::mat4 matrix);

	// Sets local position via a world position.
	// TODO: implement
	void setWorldPosition(glm::vec3 pos);

	// Sets local rotation via a world euler rotation.
	// TODO: implement
	void setWorldRotation(glm::vec3 rot);

	// Sets local scale via a world scale.
	// TODO: implement
	void setWorldScale(glm::vec3 scale);

	// precompute the world transformation matrix given a parent transformation. 
	void configureTransform(glm::mat4 parent);

	// precompute the world transformation matrix by automatically retrieving it's parent.
	void configureTransform();

	// gets the entity's static status. 
	bool getStatic() const;

	// Sets the static flag on this entity. Setting this to true will completely freeze all transforms 
	// on this entity and it's children. Moving the parent (even if not static) will not move this entity.
	// ONLY call when you're done configuring it.
	void setStatic(bool torf);

	// Inform this entity to destroy itself. 
	void destroy();

	// Release resources. Only call it if you REALLY know what you're doing.
	// Call destroy() otherwise.
	void release();

private:
	// Helper method to remove child with ID. This should only be called internally, as bindings will not be correct.
	void removeChild(unsigned int id);

	// Helper method to properly bind a parent and child entity together. 
	static void bindEntities(Entity* parent, Entity* child);

};