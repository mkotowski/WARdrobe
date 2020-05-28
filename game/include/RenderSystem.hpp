#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "stb_image.h"

#include "AssetManager.hpp"
#include "Camera.hpp"
#include "ModelArray.hpp"
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
	int 	CheckDistance(Entity entityToCheck, std::shared_ptr<ComponentManager> componentManager);
	Entity  cameraEntity;
	Window* window;
	std::map <std::string, Entity> shaders;
	float 	firstLevelOfDetail = 30.0f;
	float 	secondLevelOfDetail = 50.0f;
	float 	thirdLevelOfDetail = 75.0f;
};

void
RenderSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto const& entity : entities) {

		auto& renderer = componentManager->GetComponent<Renderer>(entity);
		auto& modelArray = componentManager->GetComponent<ModelArray>(entity);
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

		
		if (modelArray.checkLevelOfDetail == 0)
		{
			renderer.Draw(&shader,
		              &modelArray.zeroLevelModel,
		              &componentManager->GetComponent<Camera>(cameraEntity),
		              transform.position,
		              transform.rotation,
		              transform.scale,
		              this->window->GetWindowWidth(),
		              this->window->GetWindowHeight());
		}
		else
		{
			// Gets distance between camera and model
			int checkedLevelOfDetail = CheckDistance(entity, componentManager);

			
			if (checkedLevelOfDetail == 1)	// FirstLevel of detail
			{
				renderer.Draw(&shader,
		              &modelArray.firstLevelModel,
		              &componentManager->GetComponent<Camera>(cameraEntity),
		              transform.position,
		              transform.rotation,
		              transform.scale,
		              this->window->GetWindowWidth(),
		              this->window->GetWindowHeight());
			} 
			else if (checkedLevelOfDetail == 2)	// Second level of detail
			{
				renderer.Draw(&shader,
		              &modelArray.secondLevelModel,
		              &componentManager->GetComponent<Camera>(cameraEntity),
		              transform.position,
		              transform.rotation,
		              transform.scale,
		              this->window->GetWindowWidth(),
		              this->window->GetWindowHeight());
			}
			else if (checkedLevelOfDetail == 3)	// Third level of detail
			{
				renderer.Draw(&shader,
		              &modelArray.thirdLevelModel,
		              &componentManager->GetComponent<Camera>(cameraEntity),
		              transform.position,
		              transform.rotation,
		              transform.scale,
		              this->window->GetWindowWidth(),
		              this->window->GetWindowHeight());
			}
			else	// Model outside of the view
			{
				// What do you expect should be here huh?
			}			
		}		
	}
}

void
RenderSystem::Init()
{
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void
RenderSystem::Draw()
{}

int
RenderSystem::CheckDistance(Entity entityToCheck, std::shared_ptr<ComponentManager> componentManager)
{
	auto& transformOfModel = componentManager->GetComponent<Transform>(entityToCheck);
	auto& transformOfCamera = componentManager->GetComponent<Transform>(this->cameraEntity);
	float distance = std::fabs(glm::distance(transformOfModel.position, transformOfCamera.position));
	if (distance <= firstLevelOfDetail)
	{
		return 1;
	}
	else if (distance > firstLevelOfDetail && distance <= secondLevelOfDetail)
	{
		return 2;
	}
	else if (distance > secondLevelOfDetail && distance <= thirdLevelOfDetail)
	{
		return 3;
	}
	else if (distance > thirdLevelOfDetail)
	{
		return 4;
	}	
	return 4;
}