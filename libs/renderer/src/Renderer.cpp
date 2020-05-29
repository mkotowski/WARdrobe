#include "Renderer.hpp"

Renderer::Renderer() {}

Renderer::Renderer(unsigned int drawingType)
  : drawingType(drawingType)
{}

void
Renderer::Draw(Shader*   ourShader,
               Model*    ourModel,
               Camera*   sceneCamera,
               glm::vec3 position,
               glm::vec3 rotation,
               glm::vec3 scale,
               int       width,
               int       height)
{
	ourShader->use();

	glm::mat4 projection =
	  glm::perspective(glm::radians(sceneCamera->fieldOfView),
	                   (float)width / (float)height,
	                   0.1f,
	                   100.0f);
	glm::mat4 view =
	  glm::lookAt(sceneCamera->cameraPos,
	              sceneCamera->cameraPos + sceneCamera->cameraFront,
	              sceneCamera->cameraUp);

	glm::vec4 newColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, position);

	model = glm::rotate(
	  model, glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f)); // X axis
	model = glm::rotate(
	  model, glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)); // Y axis
	model = glm::rotate(
	  model, glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)); // Z axis

	model = glm::scale(model, scale);

	ourShader->setMat4("projection", projection);
	ourShader->setMat4("view", view);
	ourShader->setMat4("model", model);
	ourShader->setVec4("newColor", newColor);
	if (ourShader->shaderType != "boxShader")
		ourShader->SetSubroutineFragment(ourShader->currentSubroutine);

	if (drawingType == 1) {
		glm::mat4 mv = view * model;

		ourShader->setMat4("ModelViewMatrix", mv);
	}

	// ourShader->setInt("texture1", ourModel->textureID);

	ourModel->Draw(ourShader->ID);
}