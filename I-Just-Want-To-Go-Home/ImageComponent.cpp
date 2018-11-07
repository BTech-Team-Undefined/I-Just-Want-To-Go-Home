#include "ImageComponent.h"

#include "Core\Game.h"

ImageComponent::ImageComponent() : Component(std::type_index(typeid(ImageComponent)))
{
}


ImageComponent::~ImageComponent()
{
}

void ImageComponent::loadImage(std::string path)
{
	imageId = Game::instance().loader.LoadTexture(path, &this->width, &this->height);
}
