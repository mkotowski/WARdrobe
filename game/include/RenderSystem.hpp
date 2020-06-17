#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "stb_image.h"

#include "AssetManager.hpp"
#include "Animator.hpp"
#include "Camera.hpp"
#include "ModelArray.hpp"
#include "Renderer.hpp"
#include "Skybox.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include "Light.hpp"
#include "ecs.hpp"
#include "ColliderSystem.hpp"



class RenderSystem : public System
{
public:
	void    Update(float                             dt,
	               std::shared_ptr<ComponentManager> componentManager) override;
	void    Init();
	void    Draw();
	int 	CheckDistance(Entity entityToCheck, std::shared_ptr<ComponentManager> componentManager);
	Entity  cameraEntity;
	Entity 	lightEntity;
	Window *window;
	std::map <std::string, Entity> *shaders;
	Skybox* skybox;

	float 	firstLevelOfDetail = 30.0f;
	float 	secondLevelOfDetail = 50.0f;
	float 	thirdLevelOfDetail = 75.0f;
};

void
RenderSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{	
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto& cameraComponent = componentManager->GetComponent<Camera>(cameraEntity);
	// Setup all neded entities so they dont have to be searched and/or attached to renderSystem at the start
	for (auto& const entity: entities)
	{
		if (componentManager->GetComponent<Renderer>(entity).drawingType == 3)
		{
			this->skybox = &componentManager->GetComponent<Skybox>(entity);
		}
	}

	for (auto const& entity : entities) 
	{
		auto& renderer = componentManager->GetComponent<Renderer>(entity);
		
		auto& shader = Shader();
		if (renderer.drawingType == 0 || renderer.drawingType == 3)
		{
			shader = componentManager->GetComponent<Shader>(shaders->at("modelShader"));
			shader.use();
		}

		else if (renderer.drawingType == 1)
		{
			shader = componentManager->GetComponent<Shader>(shaders->at("billboardShader"));
			shader.use();
		}
		else if (renderer.drawingType == 2)
		{
			shader = componentManager->GetComponent<Shader>(shaders->at("animatedModelShader"));
			shader.use();
			auto& animator = componentManager->GetComponent<Animator>(entity);
			animator.PlayCurrentAnimation(dt);
		}
		else if (renderer.drawingType == 3)
		{
			shader = componentManager->GetComponent<Shader>(shaders->at("skyBoxShader"));
			shader.use();

			
			renderer.DrawSkybox(&shader, 
								this->skybox, 
								&cameraComponent, 
								window->GetWindowWidth(), 
								window->GetWindowHeight());
			continue;	
		}
		else if (renderer.drawingType == 4)
		{
			shader = componentManager->GetComponent<Shader>(shaders->at("cubemapShader"));
			shader.use();
			auto& modelArray = componentManager->GetComponent<ModelArray>(entity);	
			auto& transform = componentManager->GetComponent<Transform>(entity);
			renderer.DrawRefractiveObject(&shader, 
										  &modelArray.zeroLevelModel, 
										  &cameraComponent, 
										  this->skybox,
										  transform.position,
										  transform.rotation,
									  	  transform.scale,
										  this->window->GetWindowWidth(),
										  this->window->GetWindowHeight());
		}
		
		auto& modelArray = componentManager->GetComponent<ModelArray>(entity);	
		auto& transform = componentManager->GetComponent<Transform>(entity);

		if (modelArray.checkLevelOfDetail == 0)
		{
			renderer.Draw(&shader,
		              &modelArray.zeroLevelModel,
		              &cameraComponent,
		              transform.position,
		              transform.rotation,
		              transform.scale,
		              this->window->GetWindowWidth(),
		              this->window->GetWindowHeight());

			if (renderer.drawingType == 3) {
				shader =
				  componentManager->GetComponent<Shader>(shaders["boxShader"]);

				auto& box = componentManager->GetComponent<BoundingBox>(entity);

				DrawBoundingBox(box,
				                &componentManager->GetComponent<Camera>(cameraEntity),
				                &shader,
				                this->window);
			}
		}
		else
		{
			// Gets distance between camera and model
			int checkedLevelOfDetail = CheckDistance(entity, componentManager);

			
			if (checkedLevelOfDetail == 1)	// FirstLevel of detail
			{
				renderer.Draw(&shader,
		              &modelArray.firstLevelModel,
		              &cameraComponent,
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
		              &cameraComponent,
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
		              &cameraComponent,
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
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
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