#pragma once

#include "../Shader.h"
#include "../Material.h"

class PostProcess
{
public:
	PostProcess();
	~PostProcess();
	Shader* shader;
	Material* settings;
};

