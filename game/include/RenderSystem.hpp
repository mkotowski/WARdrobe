#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "stb_image.h"

#include "AssetManager.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include "ecs.hpp"

class RenderSystem : public System
{
public:
	void    Update(float                             dt,
	               std::shared_ptr<ComponentManager> componentManager) override;
	void    Init();
	void    Draw();
	Entity  cameraEntity;
	Window* window;
	std::map <std::string, Entity> shaders;
};

void
RenderSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto const& entity : entities) {

		auto& renderer = componentManager->GetComponent<Renderer>(entity);
		auto& model = componentManager->GetComponent<Model>(entity);
		auto& shader = Shader();
		if (renderer.drawingType == 0)
		{
			shader = componentManager->GetComponent<Shader>(shaders["modelShader"]);
		}
		else if (renderer.drawingType == 1)
		{
			shader = componentManager->GetComponent<Shader>(shaders["billboardShader"]);
		}		
		auto& transform = componentManager->GetComponent<Transform>(entity);

		renderer.Draw(&shader,
		              &model,
		              &componentManager->GetComponent<Camera>(cameraEntity),
		              transform.position,
		              transform.rotation,
		              transform.scale,
		              this->window->GetWindowWidth(),
		              this->window->GetWindowHeight());
	}

	//std::cout << "Rendering updated\n";
}

void
RenderSystem::Init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void
RenderSystem::Draw()
{}