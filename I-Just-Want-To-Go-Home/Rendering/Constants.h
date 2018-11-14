#pragma once

#include <string>

// General 
extern const std::string SHADER_MODEL;
extern const std::string SHADER_VIEW;
extern const std::string SHADER_PROJECTION;
extern const std::string SHADER_MODELVIEW;
// Colors
extern const std::string SHADER_DIFFUSE;
extern const std::string SHADER_SPECULAR;
extern const std::string SHADER_AMBIENT;
extern const std::string SHADER_EMISSIVE;
extern const std::string SHADER_SHININESS;
// Textures 
extern const std::string SHADER_TEX_NONE;
extern const std::string SHADER_TEX_DIFFUSE;
extern const std::string SHADER_TEX_SPECULAR;
extern const std::string SHADER_TEX_NORMAL;
extern const std::string SHADER_TEX_HEIGHT;
// Lighting 
extern const std::string SHADER_LIGHT_D_POS;
extern const std::string SHADER_LIGHT_D_DIR;
extern const std::string SHADER_LIGHT_D_COLOR;


class Constants
{
public:
	Constants() {};
	~Constants() {};
};

