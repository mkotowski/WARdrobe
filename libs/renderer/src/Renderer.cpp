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
	glm::mat4 projection =
	glm::perspective(glm::radians(sceneCamera->fieldOfView),
					(float)width / (float)height,
					0.1f,
					100.0f);
	glm::mat4 view =
	glm::lookAt(sceneCamera->cameraPos,
				sceneCamera->cameraPos + sceneCamera->cameraFront,
				sceneCamera->cameraUp);

	glm::vec4 newColor = ourModel->color;

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
	
	if (ourShader->shaderType == "modelShader" || ourShader->shaderType == "animatedModelShader")
	{
		ourShader->setVec3("newColor", this->newColor);
		ourShader->SetSubroutineFragment(ourShader->currentSubroutine);
	}
		

	if (drawingType == 1) {
		glm::mat4 mv = view * model;

		ourShader->setMat4("ModelViewMatrix", mv);
	}
	
	ourModel->Draw(ourShader->ID);
}

void
Renderer::DrawToShadowMap(Shader*   ourShader,
               Model*    ourModel,
               glm::vec3 position,
               glm::vec3 rotation,
               glm::vec3 scale)
{
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, position);

	model = glm::rotate(
	model, glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f)); // X axis
	model = glm::rotate(
	model, glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)); // Y axis
	model = glm::rotate(
	model, glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)); // Z axis

	model = glm::scale(model, scale);

	ourShader->setMat4("model", model);
	
	ourModel->Draw(ourShader->ID);
}

void
Renderer::DrawSkybox(Shader* ourShader, Skybox* ourSkybox, Camera* ourCamera, int width, int height)
{
	
	glm::mat4 projection = 
	glm::perspective(glm::radians(ourCamera->fieldOfView),
	 				(float)width / (float)height,
	 				0.1f,
	 				100.0f);
	glm::mat4 view =
	glm::lookAt(glm::vec3(0.0f),
				glm::vec3(0.0f) + ourCamera->cameraFront,
				ourCamera->cameraUp);

	ourShader->setMat4("view", view);
	ourShader->setMat4("projection", projection);
	ourShader->setInt("skybox", 0);
	ourSkybox->DrawSkybox(ourShader->ID);
}

void
Renderer::DrawRefractiveObject(Shader*   ourShader,
								Model*    ourModel,
								Camera*   sceneCamera,
								Skybox*	  skybox,
								glm::vec3 position,
								glm::vec3 rotation,
								glm::vec3 scale,
								int       width,
								int       height)
{
	glm::mat4 projection =
	glm::perspective(glm::radians(sceneCamera->fieldOfView),
					(float)width / (float)height,
					0.1f,
					100.0f);

		glm::mat4 view =
		glm::lookAt(sceneCamera->cameraPos,
				sceneCamera->cameraPos + sceneCamera->cameraFront,
				sceneCamera->cameraUp);


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
	ourShader->setVec3("cameraPos", sceneCamera->cameraPos);
	skybox->ActivateTexture();
	ourShader->setInt("skybox", 0);
	ourModel->DrawAsRefractive(ourShader->ID);
}