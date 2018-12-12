#include "PhysicsVector.h"

PhysicsVector::PhysicsVector() : x(0), y(0) {}

PhysicsVector::PhysicsVector(float x, float y) : x(x), y(y) {}

PhysicsVector::PhysicsVector(const PhysicsVector& v)
{
	this->x = v.x;
	this->y = v.y;
}

PhysicsVector PhysicsVector::operator=(const PhysicsVector& v2)
{
	this->x = v2.x;
	this->y = v2.y;
	return *this;
}

PhysicsVector PhysicsVector::operator+(const PhysicsVector& v2) const
{
	PhysicsVector v(this->x + v2.x, this->y + v2.y);
	return v;
}

PhysicsVector PhysicsVector::operator-(const PhysicsVector& v2) const
{
	PhysicsVector v(this->x - v2.x, this->y - v2.y);
	return v;
}

PhysicsVector PhysicsVector::operator-() const
{
	PhysicsVector v(-this->x, -this->y);
	return v;
}

PhysicsVector PhysicsVector::operator+=(const PhysicsVector& v2) {
	this->x += v2.x;
	this->y += v2.y;
	PhysicsVector v(this->x, this->y);
	return v;
}

PhysicsVector PhysicsVector::operator-=(const PhysicsVector& v2) {
	this->x -= v2.x;
	this->y -= v2.y;
	PhysicsVector v(this->x, this->y);
	return v;
}

PhysicsVector PhysicsVector::operator*(const float& f) const {
	PhysicsVector v(this->x * f, this->y * f);
	return v;
}

PhysicsVector PhysicsVector::operator*=(const float& f) {
	this->x *= f;
	this->y *= f;
	PhysicsVector v(this->x, this->y);
	return v;
}

float PhysicsVector::dot(const PhysicsVector& v) const
{
	return this->x * v.x + this->y * v.y;
}

float PhysicsVector::project(const PhysicsVector& v2) const
{
	try {
		return this->dot(v2.unit());
	}
	catch (std::exception& _) {
		throw std::exception("Cannot project onto vector (0, 0)");
	}
}

PhysicsVector PhysicsVector::unit() const
{
	if (this->x == 0 && this->y == 0) {
		throw std::exception("Unit vector of (0, 0) does not exist.");
	}
	float f = this->length();
	PhysicsVector v(this->x / f, this->y / f);
	return v;
}

float PhysicsVector::length() const
{
	return std::sqrtf(this->x * this->x + this->y * this->y);
}

PhysicsVector::~PhysicsVector()
{
}

PhysicsVector operator*(const float& f, const PhysicsVector& v)
{
	PhysicsVector v2 = v * f;
	return v2;
}