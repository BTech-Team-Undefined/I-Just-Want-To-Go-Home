#pragma once

#include <ft2build.h>
#include <freetype\freetype.h>

class TextComponent
{
public:
	TextComponent();
	~TextComponent();
	void foobar()
	{
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{

		}
	}
};

