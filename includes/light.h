#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "common.h"

class DirectionalLight {
public:
	float azimuth;
	float elevation;
	glm::vec3 lightDir; // direction of light. If elevation is 90, it would be (0,-1,0)
	glm::vec3 lightColor; // this is I_d (I_s = I_d, I_a = 0.3 * I_d)

	DirectionalLight(float azimuth, float elevation, glm::vec3 lightColor);

	DirectionalLight(glm::vec3 lightDir, glm::vec3 lightColor);

	glm::mat4 getViewMatrix(glm::vec3 cameraPosition);

	glm::mat4 getProjectionMatrix();

	void updateLightDir();

	void processKeyboard(float xoffset, float yoffset);
};


#endif