#pragma once

#include <string>
#include <glm\glm.hpp>
#include <ft2build.h>
#include <freetype\freetype.h>
#include "EntitySystems\Component.h"
// shader

// Display a text message. Always renders on top. 
// Adjust entity's position to move component. rotation and scale do nothing.
// No sorting functionality. 
class TextComponent : public Component
{
public:
	TextComponent();
	~TextComponent();
	
	// set the text to be displayed
	void setText(std::string s);
	
	// returns the current text
	std::string getText();
	
	// clears the text
	void clear();
	
	// 2d pixel position of text 
	glm::vec2 getPosition();
	
	// size of the text. cannot be below 0.
	float scale = 1.0f;
	
	// font of the text. remember to use the font path (not name).
	std::string font;	

	// color of the text. 
	glm::vec3 color;
	
	// custom shader for this text. leave as nullptr to use default shader.
	// Shader* customShader;

	virtual void update(float dt) override {};

private:
	std::string text; 
};

