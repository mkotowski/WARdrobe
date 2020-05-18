#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "glm/gtc/type_ptr.hpp"

class Renderer
{
public:
	Renderer();
	Renderer(unsigned int);
	Renderer(bool, float[], int, unsigned int[], int, std::string);
	void
	     Draw(Shader*, Model*, Camera*, glm::vec3, glm::vec3, glm::vec3, int, int, float);
	void Draw(Shader *);
	void Init();

	unsigned int drawingType;

private:
};