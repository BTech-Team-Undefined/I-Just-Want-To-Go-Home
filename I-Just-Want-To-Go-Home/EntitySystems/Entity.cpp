#include "Entity.h"

unsigned int Entity::_curID = 0;

Entity::Entity() :_id(Entity::_curID)
{
	Entity::_curID++;

}

Entity::Entity(Entity* parent) :_id(Entity::_curID)
{
	Entity::_curID++;
	setParent(parent);
}

void Entity::update(float dt)
{
	for (const auto& c : _components)
		c.second->update(dt);
	for (const auto& c : _children)
		c->update(dt);
}

unsigned int Entity::getID() const
{
	return _id;
}

void Entity::setParent(Entity* parent)
{
	_parent = parent;
	_parent->addChild(this);
}

Entity* Entity::getParent()
{
	return _parent;
}

void Entity::addChild(Entity* child)
{
	child->_parent = this;
	_children.push_back(child);
}

//returns child with id
Entity* Entity::getChild(unsigned int id)
{
	return *std::find_if(_children.begin(), _children.end(), [id](Entity* e) { return e->getID() == id; });
}

//removes child with id
void Entity::removeChild(unsigned int id)
{
	auto t = std::find_if(_children.begin(), _children.end(), [&id](const Entity* e) { return e->getID() == id; });
	_children.erase(t);
	(*t)->setParent(nullptr);
}

glm::mat4 Entity::getLocalTransformation()
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
	model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
	model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
	model = glm::scale(model, scale);
	return model;
}

glm::mat4 Entity::getWorldTransformation()
{
	glm::mat4 transform = getLocalTransformation();
	if (getParent() == nullptr)
		return transform;
	else
		return transform * getParent()->getWorldTransformation();
}

void Entity::setLocalTransform(glm::mat4 matrix)
{
	// warning: assuming matrix bottom-right value is 1. If not divide everything in matrix by that value. 
	glm::quat rot;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(matrix, scale, rot, position, skew, perspective);
	rotation = glm::eulerAngles(rot);
}

std::vector<Entity*> const& Entity::getChildren() const
{
	return _children;
}