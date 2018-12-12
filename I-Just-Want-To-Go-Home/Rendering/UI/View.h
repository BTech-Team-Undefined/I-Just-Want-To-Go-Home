#pragma once
class View
{
public:
	View();
	~View();

	// lower numbers are rendered first. 
	int layer = 0;

	// a comparison function for std::sort 
	static bool compare(const View& lhs, const View& rhs);

	// a comparison function for std::sort 
	static bool comparePointers(const View* lhs, const View* rhs);
};

