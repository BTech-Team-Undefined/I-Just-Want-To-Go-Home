#include "Camera.h"

#include <GL\glad.h>
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"


glm::mat4 Camera::GetProjectionMatrix()
{
	if (isOrtho)
		return glm::ortho(-fov * aspect, fov * aspect, -fov, fov, nearPlane, farPlane);
	else
		return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

glm::mat4 Camera::GetViewMatrix()
{
	glm::mat4 v = glm::mat4(1.0f);	// this is the identity matrix 
	v = glm::translate(v, GetEntity()->position);
	v = glm::rotate(v, GetEntity()->rotation.z, glm::vec3(0, 0, 1));
	v = glm::rotate(v, GetEntity()->rotation.y, glm::vec3(0, 1, 0));
	v = glm::rotate(v, GetEntity()->rotation.x, glm::vec3(1, 0, 0));
	return glm::inverse(v);

}
