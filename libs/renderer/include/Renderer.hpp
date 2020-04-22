#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "Shader.hpp"

class Renderer
{
public:
	Renderer();
	Renderer(unsigned int);
	Renderer(bool, float[], int, unsigned int[], int, std::string);
	void
	     Draw(Shader*, Model*, Camera*, glm::vec3, glm::vec3, glm::vec3, int, int);
	void Init();

	unsigned int drawingType;

private:

};