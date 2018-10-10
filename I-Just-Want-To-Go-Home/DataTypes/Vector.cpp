#include "Vector.h"

Vector::Vector(): x(0), y(0) {}

Vector::Vector(float x, float y): x(x), y(y) {}

Vector::Vector(const Vector& v)
{
	this->x = v.x;
	this->y = v.y;
}

Vector Vector::operator=(const Vector& v2) const
{
	Vector v(v2.x, v2.y);
	return v;
}

Vector Vector::operator+(const Vector& v2) const 
{
	Vector v(this->x + v2.x, this->y + v2.y);
	return v;
}

Vector Vector::operator-(const Vector& v2) const
{
	Vector v(this->x - v2.x, this->y - v2.y);
	return v;
}

Vector Vector::operator-() const
{
	Vector v(-this->x, -this->y);
	return v;
}

Vector Vector::operator+=(const Vector& v2) {
	this->x += v2.x;
	this->y += v2.y;
	Vector v(this->x, this->y);
	return v;
}

Vector Vector::operator-=(const Vector& v2) {
	this->x -= v2.x;
	this->y -= v2.y;
	Vector v(this->x, this->y);
	return v;
}

Vector Vector::operator*(const float& f) const {
	Vector v(this->x * f, this->y * f);
	return v;
}

Vector Vector::operator*=(const float& f) {
	this->x *= f;
	this->y *= f;
	Vector v(this->x, this->y);
	return v;
}

float Vector::dot(const Vector& v) const
{
	return this->x * v.x + this->y * v.y;
}

float Vector::project(const Vector& v2) const
{
	try {
		return this->dot(v2.unit());
	}
	catch (std::exception& _) {
		throw std::exception("Cannot project onto vector (0, 0)");
	}
}

Vector Vector::unit() const
{
	if (this->x == 0 && this->y == 0) {
		throw std::exception("Unit vector of (0, 0) does not exist.");
	}
	float f = this->length();
	Vector v(this->x / f, this->y / f);
	return v;
}

float Vector::length() const
{
	return std::sqrtf(this->x * this->x + this->y * this->y);
}

Vector::~Vector()
{
}

Vector operator*(const float& f, const Vector& v)
{
	Vector v2 = v * f;
	return v2;
}
