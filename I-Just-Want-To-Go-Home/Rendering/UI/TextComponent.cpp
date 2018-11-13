#include "TextComponent.h"

#include "..\..\EntitySystems\Entity.h"

TextComponent::TextComponent() : Component(std::type_index(typeid(TextComponent)))
{
}

TextComponent::~TextComponent()
{
}

void TextComponent::setText(std::string s)
{
	text = s;
}

std::string TextComponent::getText()
{
	return text;
}

void TextComponent::clear()
{
	text = "";
}

glm::vec2 TextComponent::getPosition()
{
	return getEntity()->getWorldPosition();
}
