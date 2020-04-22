#include "Renderer.hpp"

Renderer::Renderer()
  : billboard(false)
{}

Renderer::Renderer(bool         billboard,
                   float        vertices[],
                   int          verticesSize,
                   unsigned int indices[],
                   int          indicesSize)
  : billboard(billboard)
{
	if (!billboard)
		return;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
	  GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesSize, indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(
	  1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(
	  2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// load and create a texture
	// -------------------------
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D,
	              texture); // all upcoming GL_TEXTURE_2D operations now have
	                        // effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(
	  GL_TEXTURE_2D,
	  GL_TEXTURE_WRAP_S,
	  GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can
	// find files on any IDE/platform; replace it with your own image path.
	unsigned char* data =
	  stbi_load("assets/textures/container.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D,
		             0,
		             GL_RGB,
		             width,
		             height,
		             0,
		             GL_RGB,
		             GL_UNSIGNED_BYTE,
		             data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

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
	// ourShader->setInt("texture1", ourModel->textureID);

	ourModel->Draw(ourShader->ID);
}

void
Renderer::Draw(Shader*   ourShader,
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

	glm::mat4 mv = view * model;

	ourShader->setMat4("ModelViewMatrix", mv);

	ourShader->setVec4("newColor", newColor);
	ourShader->setInt("texture1", 0);

	// draw geometry

	// bind Texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// render container
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool
Renderer::IsBillboard()
{
	return billboard;
}