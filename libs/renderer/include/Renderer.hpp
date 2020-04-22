#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "Shader.hpp"

class Renderer
{
public:
	Renderer();
	Renderer(bool, float[], int, unsigned int[], int);
	void
	     Draw(Shader*, Model*, Camera*, glm::vec3, glm::vec3, glm::vec3, int, int);
	void Draw(Shader*, Camera*, glm::vec3, glm::vec3, glm::vec3, int, int);
	// void Draw(Shader *, Model *, unsigned int);
	void Init();
	bool IsBillboard();

private:
	unsigned int VAO, VBO, EBO, texture;
	bool         billboard;
};