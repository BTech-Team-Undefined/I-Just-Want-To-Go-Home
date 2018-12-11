#pragma once

enum class TextAlignment { Left, Center, Right };

class View
{
public:
	View();
	~View();

	// lower numbers are rendered first. 
	int layer = 0;

	// alignment of UI component
	TextAlignment alignment = TextAlignment::Left;

	// a comparison function for std::sort 
	static bool compare(const View& lhs, const View& rhs);

	// a comparison function for std::sort 
	static bool comparePointers(const View* lhs, const View* rhs);
};

