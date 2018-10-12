#include <cmath>
#include "Point.h"

using namespace std;

Point::Point(float x, float y)
{
	this->x = x;
	this->y = y;
}

Point::~Point()
{
}

void Point::Set(float x, float y)
{
	this->x = x;
	this->y = y;
}

float Point::Distance(Point p)
{
	float x1 = this->x;
	float y1 = this->y;
	float x2 = p.x;
	float y2 = p.y;
	return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

bool Point::Near(Point p)
{
	return this->Near(p, 0.2f);
}

bool Point::Near(Point p, float distance)
{
	return this->Distance(p) < distance;
}

Point Point::operator+(const Point& p)
{
	Point newPoint(this->x, this->y);
	newPoint.x += p.x;
	newPoint.y += p.y;
	return newPoint;
}
