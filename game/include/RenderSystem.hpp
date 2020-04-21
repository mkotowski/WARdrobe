#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "stb_image.h"

#include "ecs.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "AssetManager.hpp"
#include "Window.hpp"

class RenderSystem : public System
{
public:
	void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void Init();
	void Draw();
	Entity cameraEntity;
	Window * window;
};

void
RenderSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (auto const& entity : entities) {
		
		auto& renderer = componentManager->GetComponent<Renderer>(entity);
		auto& shader = componentManager->GetComponent<Shader>(entity);
		auto& model = componentManager->GetComponent<Model>(entity);
		auto& transform = componentManager->GetComponent<Transform>(entity);
		
		renderer.Draw(&shader, &model, 
		&componentManager->GetComponent<Camera>(cameraEntity), 
		transform.position,
		transform.rotation,
		transform.scale,
		this->window->GetWindowWidth(),
		this->window->GetWindowHeight()
		);	
			 	 	
	}

	std::cout << "Rendering updated\n";
}

void
RenderSystem::Init()
{
	glEnable(GL_DEPTH_TEST);
}

void
RenderSystem::Draw()
{

}