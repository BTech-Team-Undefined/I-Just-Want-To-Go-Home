#pragma once
class Vector
{
public:
	Vector(float x, float y);
	void operator=(const Vector &v);
	Vector operator+(const Vector& v);
	Vector operator-(const Vector& v);
	Vector& operator+=(const Vector& v);
	Vector& operator-=(const Vector& v);
	Vector operator*(const float& f);
	Vector& operator*=(const float& f);
	Vector dot(const Vector& v);
	Vector project(const Vector& v);
	Vector getUnitVector();
	float getLength();
	const float x;
	const float y;
	~Vector();
};

