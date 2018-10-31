#pragma once

using namespace std;

class Point
{
public:
	Point(float x, float y);
	~Point();

	void Set(float x, float y);
	bool Near(Point p);
	bool Near(Point p, float distance);
	float Distance(Point p);
	Point operator+(const Point& p);
	Point Rotate(float angle);

	float x;
	float y;
};

