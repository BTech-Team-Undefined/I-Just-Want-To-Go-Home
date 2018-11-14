#include "View.h"



View::View()
{
}


View::~View()
{
}

bool View::compare(const View & lhs, const View & rhs)
{
	return rhs.layer < lhs.layer;
}

bool View::comparePointers(const View * lhs, const View * rhs)
{
	return rhs->layer < lhs->layer;
}
