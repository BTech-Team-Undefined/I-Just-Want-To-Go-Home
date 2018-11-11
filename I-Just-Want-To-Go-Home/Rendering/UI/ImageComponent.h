#pragma once

#include <string>
#include <glm\glm.hpp>
#include "..\..\EntitySystems\Entity.h"
#include "..\..\EntitySystems\Component.h"
#include "View.h"

class ImageComponent : public Component, public View
{
public:
	ImageComponent();
	~ImageComponent();

	// load the image to be used for this component
	void loadImage(std::string path);

	glm::vec2 getPosition()
	{
		return getEntity()->getWorldPosition();
	}

	void update(float dt) override {};

	unsigned int imageId = -1;
	int width;	// not used 
	int height;	// not used 
	float opacity = 1.0f; 
};

