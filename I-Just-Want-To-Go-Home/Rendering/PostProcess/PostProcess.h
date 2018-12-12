#pragma once

#include "../Shader.h"
#include "../Material.h"
#include <memory>

class PostProcess
{
public:
	PostProcess();
	~PostProcess();
	bool enabled = true;
	std::unique_ptr<Shader> shader;
	std::unique_ptr<Material> settings;
};

