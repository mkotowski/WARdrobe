#include "Renderer.hpp"

Renderer::Renderer()
{

}

void Renderer::Draw(Shader *ourShader, Model *ourModel, unsigned int textureID)
{
    ourShader->use();
    //Camera - temporary here, will be removed
    glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 30.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f); 
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec4 newColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);
    ourShader->setMat4("model", model);
    ourShader->setVec4("newColor", newColor);
    ourShader->setInt("texture1", textureID);

    ourModel->Draw(ourShader->ID);
}
