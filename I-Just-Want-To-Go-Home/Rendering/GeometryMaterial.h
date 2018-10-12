#pragma once

#include "Material.h"

// Basic material for the geometry shader. 
class GeometryMaterial : Material
{
public:
	void LoadAlbedoMap(const char* path);
	void LoadNormalMap(const char* path);
	void SetSpecular(float level); 
};

