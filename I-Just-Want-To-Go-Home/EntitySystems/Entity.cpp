#include "Entity.h"

#include "../Core/Game.h"

unsigned int Entity::_curID = 0;

Entity::Entity() :_id(++Entity::_curID) { }

Entity::Entity(Entity* parent) :_id(++Entity::_curID) 
{
	setParent(parent);
}

Entity::Entity(unsigned int id) : _id(id)
{
	std::cout << "Entity created with custom ID: " << _id << std::endl;
}

Entity::~Entity()
{
	std::cout << "Entity: " << _id << " destroyed" << std::endl;
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

bool Entity::getEnabled() const
{
	return _enabled;
}

bool Entity::setEnabled(bool enabled)
{
	return _enabled = enabled;
}

void Entity::setParent(Entity* parent)
{
	Entity::bindEntities(parent, this);
}

Entity* Entity::getParent() const
{
	return _parent;
}

void Entity::addChild(Entity* child)
{
	Entity::bindEntities(this, child);
}

Entity* Entity::getChild(unsigned int id)
{
	return *std::find_if(_children.begin(), _children.end(), [id](Entity* e) { return e->getID() == id; });
}

void Entity::removeChild(unsigned int id)
{
	auto t = std::find_if(_children.begin(), _children.end(), [&id](const Entity* e) { return e->getID() == id; });
	_children.erase(t);
}

void Entity::bindEntities(Entity * parent, Entity * child)
{
	if (!child) return;
	
	if (child->_parent)
		child->_parent->removeChild(child->_id);

	child->_parent = parent;

	if (parent)
		parent->_children.push_back(child);
}


glm::mat4 Entity::getLocalTransformation()
{
	// we going super-sonic http://www.opengl-tutorial.org/assets/faq_quaternions/index.html#Q26
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	auto A = glm::cos(rotation.x);
	auto B = glm::sin(rotation.x);
	auto C = glm::cos(rotation.y);
	auto D = glm::sin(rotation.y);	
	auto E = glm::cos(rotation.z);
	auto F = glm::sin(rotation.z);
	auto BC = B * C;
	auto BD = B * D;

	glm::mat4 rot = glm::mat4(0.0f);	// this is the precalculated y*x*z rotation matrix. (x2 faster)
	rot[0][0] =  C * E + BD * F;
	rot[0][1] =  A * F;
	rot[0][2] = -D * E + BC * F;
	rot[1][0] = -C * F + BD * E;
	rot[1][1] =  A * E;
	rot[1][2] =  D * F + BC * E;
	rot[2][0] =  A * D;
	rot[2][1] = -B;
	rot[2][2] =  A * C;
	rot[3][3] =  1.0f;
	model = model * rot;

	model = glm::scale(model, scale);
	return model;
}

glm::mat4 Entity::getWorldTransformation()
{
	return _worldTransformation;
}

glm::vec3 Entity::getWorldPosition()
{
	auto ret = position;
	auto e = getParent();
	while (e != nullptr)
	{
		ret += e->position;
		e = e->getParent();
	}
	return ret;
}

glm::vec3 Entity::getWorldRotation()
{
	auto ret = rotation;
	auto e = getParent();
	while (e != nullptr)
	{
		ret += e->rotation;
		e = getParent();
	}
	return ret;
}

glm::vec3 Entity::getWorldScale()
{
	auto ret = scale;
	auto e = getParent();
	while (e != nullptr)
	{
		ret += e->scale;
		e = getParent();
	}
	return ret;
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

void Entity::configureTransform(glm::mat4 parent)
{
	_worldTransformation = parent * getLocalTransformation();
}

void Entity::configureTransform()
{
	glm::mat4 transform = getLocalTransformation();
	_worldTransformation = (getParent() == nullptr) ? transform : getParent()->getWorldTransformation() * transform;
}

void Entity::destroy()
{
	Game::instance().deleteEntity(this);
}

void Entity::release()
{
	// aka. murder_all_children_and_commit_suicide_but_remember_to_leave_a_note_for_your_parents()

	// 1. murder the children 
	for (int i = 0; i < _children.size(); i++)
	{
		_children[i]->release();
	}

	// 2. leave a note 
	if (_parent) _parent->removeChild(_id);

	// 3. commit sudoku
	delete(this);
}
