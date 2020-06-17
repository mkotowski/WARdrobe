#pragma once

#include "Camera.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Skybox.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/glm.hpp"

class Renderer
{
public:
	Renderer();
	Renderer(unsigned int);
	Renderer(bool, float[], int, unsigned int[], int, std::string);
	void
	     Draw(Shader*, Model*, Camera*, glm::vec3, glm::vec3, glm::vec3, int, int);
	void
	     DrawToShadowMap(Shader*, Model*, glm::vec3, glm::vec3, glm::vec3);
	void Draw(Shader *);
	void DrawRefractiveObject(Shader*, Model*, Camera*, Skybox* , glm::vec3, glm::vec3, glm::vec3, int, int);
	void DrawSkybox(Shader*, Skybox*, Camera*, int, int);
	void Init();

	unsigned int drawingType;
	glm::vec3 newColor = glm::vec3(1.0f, 1.0f, 1.0f);

private:
};