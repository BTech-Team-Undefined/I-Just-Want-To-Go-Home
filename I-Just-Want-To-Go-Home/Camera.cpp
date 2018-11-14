#include "Camera.h"

#include <GL\glad.h>
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include <iostream>


glm::mat4 Camera::GetProjectionMatrix()
{
	if (isOrtho)
		return glm::ortho(-fov * aspect, fov * aspect, -fov, fov, nearPlane, farPlane);
	else
		return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

glm::mat4 Camera::GetViewMatrix()
{
	auto model = getEntity()->getWorldTransformation();	// this also has scale - be warned
	return glm::inverse(model); 
}
