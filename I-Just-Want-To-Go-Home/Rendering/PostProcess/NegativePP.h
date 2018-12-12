#pragma once

#include "PostProcess.h"
#include "../Shader.h"
#include "../Material.h"

class NegativePP : public PostProcess
{
public:
	NegativePP() : PostProcess()
	{
		shader = std::make_unique<Shader>("shaders/pp_base_vertex.glsl", "shaders/pp_negative_fragment.glsl");
		settings = std::make_unique<Material>();
	}
	~NegativePP() {};
};

