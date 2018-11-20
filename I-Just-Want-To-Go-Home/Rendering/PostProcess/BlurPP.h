#pragma once

#include "PostProcess.h"
#include "../Shader.h"
#include "../Material.h"

class BlurPP : public PostProcess
{
public:
	BlurPP() : PostProcess()
	{
		shader = new Shader("shaders/pp_base_vertex.glsl", "shaders/pp_blur_fragment.glsl");
		settings = new Material();
		settings->SetBool("u_Horizontal", true);
	}
	~BlurPP() {};
};

