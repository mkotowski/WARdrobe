#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "stb_image.h"

#include "Animator.hpp"
#include "AssetManager.hpp"
#include "Camera.hpp"
#include "ColliderSystem.hpp"
#include "Light.hpp"
#include "ModelArray.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Skybox.hpp"
#include "Window.hpp"
#include "ecs.hpp"

class RenderSystem : public System
{
public:
	void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void RenderSystem::Draw(float                             dt,
	                        std::shared_ptr<ComponentManager> componentManager);

	void Init();
	int  CheckDistance(Entity                            entityToCheck,
	                   std::shared_ptr<ComponentManager> componentManager);

	void CreateColorAttachmentTexture();

	Entity  cameraEntity;
	Entity  lightEntity;
	Window* window;
	Skybox* skybox;

	std::map<std::string, Entity>* shaders;

	float firstLevelOfDetail = 30.0f;
	float secondLevelOfDetail = 50.0f;
	float thirdLevelOfDetail = 75.0f;

	unsigned int FBO, RBO;
	unsigned int quadVAO, quadVBO;
	unsigned int textureColorbuffer;
	
};

void
RenderSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{}

void
RenderSystem::Init()
{

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float quadVertices[] = 
	{ 
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

	glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// Framebuffer to texture
	
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // create a color attachment texture
    CreateColorAttachmentTexture();
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 2048, 2048); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
RenderSystem::Draw(float dt, std::shared_ptr<ComponentManager> componentManager)
{
	// first pass - to framebuffer
	//
	glViewport(0, 0, 2048, 2048);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto& cameraComponent = componentManager->GetComponent<Camera>(cameraEntity);
	// Setup all neded entities so they dont have to be searched and/or attached
	// to renderSystem at the start
	for (auto& entity : entities) {
		if (componentManager->GetComponent<Renderer>(entity).drawingType == 3) {
			this->skybox = &componentManager->GetComponent<Skybox>(entity);
		}
	}

	for (auto& entity : entities) {
		auto& renderer = componentManager->GetComponent<Renderer>(entity);

		auto& shader = Shader();

		switch(renderer.drawingType)
		{
			// Bounding box
			case 7:
				shader = componentManager->GetComponent<Shader>(shaders->at("boxShader"));
				DrawBoundingBox(componentManager->GetComponent<BoundingBox>(entity),
								&componentManager->GetComponent<Camera>(cameraEntity),
								&shader,
								this->window);
				continue;
				break;

			// Model shader with BB
			case 6:
				shader =
				componentManager->GetComponent<Shader>(shaders->at("modelShader"));
				shader.use();
				break;
			
			// Animated model with BB
			case 5:
				shader = componentManager->GetComponent<Shader>(
			  	shaders->at("animatedModelShader"));
				shader.use();
				componentManager->GetComponent<Animator>(entity).PlayCurrentAnimation(dt);
				break;


			// Refractive model
			case 4:
				shader =
				componentManager->GetComponent<Shader>(shaders->at("cubemapShader"));
				shader.use();
				renderer.DrawRefractiveObject(&shader,
											&componentManager->GetComponent<ModelArray>(entity).zeroLevelModel,
											&cameraComponent,
											this->skybox,
											componentManager->GetComponent<Transform>(entity).position,
											componentManager->GetComponent<Transform>(entity).rotation,
											componentManager->GetComponent<Transform>(entity).scale,
											this->window->GetWindowWidth(),
											this->window->GetWindowHeight());
				continue;
				break;
			
			// Skybox
			case 3:
				shader =
			  	componentManager->GetComponent<Shader>(shaders->at("skyBoxShader"));
				shader.use();
				renderer.DrawSkybox(&shader,
									this->skybox,
									&cameraComponent,
									window->GetWindowWidth(),
									window->GetWindowHeight());
				continue;
				break;

			// Animated model without BB
			case 2:
				shader = componentManager->GetComponent<Shader>(
			  	shaders->at("animatedModelShader"));
				shader.use();
				componentManager->GetComponent<Animator>(entity).PlayCurrentAnimation(dt);
				break;

			// Billboard
			case 1:
				shader =
			  	componentManager->GetComponent<Shader>(shaders->at("billboardShader"));
				shader.use();
				break;

			// Model without BB
			case 0:
				shader =
				componentManager->GetComponent<Shader>(shaders->at("modelShader"));
				shader.use();
				break;		

		}

		auto& modelArray = componentManager->GetComponent<ModelArray>(entity);
		auto& transform = componentManager->GetComponent<Transform>(entity);

		if (modelArray.checkLevelOfDetail == 0) {
			renderer.Draw(&shader,
			              &modelArray.zeroLevelModel,
			              &cameraComponent,
			              transform.position,
			              transform.rotation,
			              transform.scale,
			              this->window->GetWindowWidth(),
			              this->window->GetWindowHeight());

			if (renderer.drawingType == 6 || renderer.drawingType == 5) {
				shader =
				  componentManager->GetComponent<Shader>(shaders->at("boxShader"));

				auto& box = componentManager->GetComponent<BoundingBox>(entity);

				DrawBoundingBox(box,
				                &componentManager->GetComponent<Camera>(cameraEntity),
				                &shader,
				                this->window);
			}
		} else {
			// Gets distance between camera and model
			int checkedLevelOfDetail = CheckDistance(entity, componentManager);

			if (checkedLevelOfDetail == 1) // FirstLevel of detail
			{
				renderer.Draw(&shader,
				              &modelArray.firstLevelModel,
				              &cameraComponent,
				              transform.position,
				              transform.rotation,
				              transform.scale,
				              this->window->GetWindowWidth(),
				              this->window->GetWindowHeight());
			} else if (checkedLevelOfDetail == 2) // Second level of detail
			{
				renderer.Draw(&shader,
				              &modelArray.secondLevelModel,
				              &cameraComponent,
				              transform.position,
				              transform.rotation,
				              transform.scale,
				              this->window->GetWindowWidth(),
				              this->window->GetWindowHeight());
			} else if (checkedLevelOfDetail == 3) // Third level of detail
			{
				renderer.Draw(&shader,
				              &modelArray.thirdLevelModel,
				              &cameraComponent,
				              transform.position,
				              transform.rotation,
				              transform.scale,
				              this->window->GetWindowWidth(),
				              this->window->GetWindowHeight());
			} else // Model outside of the view
			{
				// What do you expect should be here huh?
			}
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, window->GetWindowWidth(), window->GetWindowHeight());

	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT);
	auto& shader = 
	componentManager->GetComponent<Shader>(shaders->at("quadShader"));

	shader.use();
	shader.setInt("screenTexture", 0);
	shader.SetSubroutineFragment(shader.currentSubroutine);
	glBindVertexArray(0);
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int
RenderSystem::CheckDistance(Entity                            entityToCheck,
                            std::shared_ptr<ComponentManager> componentManager)
{
	auto& transformOfModel =
	  componentManager->GetComponent<Transform>(entityToCheck);
	auto& transformOfCamera =
	  componentManager->GetComponent<Transform>(this->cameraEntity);
	float distance = std::fabs(
	  glm::distance(transformOfModel.position, transformOfCamera.position));
	if (distance <= firstLevelOfDetail) {
		return 1;
	} else if (distance > firstLevelOfDetail && distance <= secondLevelOfDetail) {
		return 2;
	} else if (distance > secondLevelOfDetail && distance <= thirdLevelOfDetail) {
		return 3;
	} else if (distance > thirdLevelOfDetail) {
		return 4;
	}
	return 4;
}

void 
RenderSystem::CreateColorAttachmentTexture()
{
	glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
}