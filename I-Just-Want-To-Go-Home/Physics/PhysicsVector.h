#pragma once
#include <cmath>
#include <exception>
class PhysicsVector
{
public:
	float x;
	float y;

	PhysicsVector();
	PhysicsVector(float x, float y);
	PhysicsVector(const PhysicsVector& v);
	PhysicsVector operator=(const PhysicsVector &v);
	PhysicsVector operator+(const PhysicsVector& v) const;
	PhysicsVector operator-(const PhysicsVector& v) const;
	PhysicsVector operator-() const;
	PhysicsVector operator+=(const PhysicsVector& v);
	PhysicsVector operator-=(const PhysicsVector& v);
	PhysicsVector operator*(const float& f) const;
	PhysicsVector operator*=(const float& f);
	float dot(const PhysicsVector& v) const;
	float project(const PhysicsVector& v) const;
	PhysicsVector unit() const;
	float length() const;
	~PhysicsVector();
};

PhysicsVector operator*(const float& f, const PhysicsVector& v);
