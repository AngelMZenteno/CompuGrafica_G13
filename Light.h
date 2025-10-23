#pragma once

#include <glew.h>
#include <glm.hpp>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, 
			GLfloat aIntensity, GLfloat dIntensity);
	//función para establecer las intensidades de la luz si se requiere
	void SetIntensities(GLfloat aIntensity, GLfloat dIntensity)
	{
		ambientIntensity = aIntensity;
		diffuseIntensity = dIntensity;

		// Si las intensidades son 0
		if (aIntensity == 0.0f && dIntensity == 0.0f) {
			//apagar para quitar el specular
			color = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		else {
			color = originalColor;
		}
	}
	~Light();

protected:
	glm::vec3 color;
	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;
	glm::vec3 originalColor;
};

