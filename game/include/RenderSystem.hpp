#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "stb_image.h"

#include "ecs.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "Renderer.hpp"
#include "AssetManager.hpp"

class RenderSystem : public System
{
public:
	void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void Init();
	void Draw();

private:

};

void
RenderSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{
	for (auto const& entity : entities) {

		componentManager->GetComponent<Renderer>(entity).Draw(
			&componentManager->GetComponent<Shader>(entity),
			&componentManager->GetComponent<Model>(entity),
			0
		);
		Draw();
	}

	std::cout << "Rendering updated\n";
}

void
RenderSystem::Init()
{
	unsigned int texture1;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load("../../../assets/textures/stone2.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void
RenderSystem::Draw()
{

}