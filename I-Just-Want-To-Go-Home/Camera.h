#pragma once

#include "glm\glm.hpp"

class Camera
{
public: 
	glm::vec3 position;
	glm::vec3 rotation; 
	float aspectRatio = 1.0f;

public:
	Camera(float aspect);
	~Camera();
	glm::mat4 GetProjectionMatrix(); 
	glm::mat4 GetViewMatrix(); 
};

