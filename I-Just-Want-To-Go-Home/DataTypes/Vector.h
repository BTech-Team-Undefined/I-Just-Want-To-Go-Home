#pragma once
#include <cmath>
#include <exception>
class Vector
{
public:
	float x;
	float y;

	Vector();
	Vector(float x, float y);
	Vector(const Vector& v);
	Vector operator=(const Vector &v) const;
	Vector operator+(const Vector& v) const;
	Vector operator-(const Vector& v) const;
	Vector operator-() const;
	Vector operator+=(const Vector& v);
	Vector operator-=(const Vector& v);
	Vector operator*(const float& f) const;
	Vector operator*=(const float& f);
	float dot(const Vector& v) const;
	float project(const Vector& v) const;
	Vector unit() const;
	float length() const;
	~Vector();
};

Vector operator*(const float& f, const Vector& v);
