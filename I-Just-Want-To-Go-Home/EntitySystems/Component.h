#pragma once

class Component
{
public:
	Component() = default;
	virtual ~Component() {};
	virtual Component& operator=(const Component&) = delete;  // Disallow copying
	Component(const Component&) = delete;
	//virtual std::shared_ptr<Component> clone() = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;
	void Kill() { _isAlive = false; }

private:
	bool _isAlive = true;
};