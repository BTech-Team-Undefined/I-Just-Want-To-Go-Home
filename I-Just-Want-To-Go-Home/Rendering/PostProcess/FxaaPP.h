#pragma once

#include "PostProcess.h"
#include "../Shader.h"
#include "../Material.h"

class FxaaPP : public PostProcess
{
public:
	FxaaPP() : PostProcess()
	{
		shader = std::make_unique<Shader>("shaders/pp_base_vertex.glsl", "shaders/pp_fxaa_fragment.glsl");
		settings = std::make_unique<Material>();
	}
	~FxaaPP() {};
};

