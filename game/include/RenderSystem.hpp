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
#include "Light.hpp"
#include "ecs.hpp"



class RenderSystem : public System
{
public:
	void    Update(float                             dt,
	               std::shared_ptr<ComponentManager> componentManager) override;
	void    Init();
	void    Draw();
	int 	CheckDistance(Entity entityToCheck, std::shared_ptr<ComponentManager> componentManager);
	unsigned int LoadCubemap(vector<std::string> faces);
	Entity  cameraEntity;
	Entity 	lightEntity;
	Window *window;
	unsigned int FBO;
	unsigned int texture;
	unsigned int RBO;
	unsigned int quadVAO = 0, quadVBO, skyboxVAO, skyboxVBO, cubeVAO, cubeVBO;
	unsigned int cubemapTexture;
	void 	RenderQuad();
	void	RenderSkyBox();
	void	RenderCube();
	

	std::map <std::string, Entity> *shaders;

	float 	firstLevelOfDetail = 30.0f;
	float 	secondLevelOfDetail = 50.0f;
	float 	thirdLevelOfDetail = 75.0f;
};

void
RenderSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{

	// glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// auto& dirLight = componentManager->GetComponent<Light>(lightEntity);
	// glm::mat4 lightProjection, lightView;
    // glm::mat4 lightSpaceMatrix;
    // float near_plane = 1.0f, far_plane = 17.5f;
    // lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    // lightView = glm::lookAt(dirLight.position, dirLight.direction, glm::vec3(0.0, 1.0, 0.0));
    // lightSpaceMatrix = lightProjection * lightView;
	
	// glViewport(0, 0, 1024, 1024);
 
	// glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// glClear(GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
	
	// // First Pass
	// for (auto const& entity : entities) 
	// {
	// 	auto& renderer = componentManager->GetComponent<Renderer>(entity);
	// 	auto& modelArray = componentManager->GetComponent<ModelArray>(entity);
	// 	auto& shader = Shader();
	// 	if (renderer.drawingType == 0)
	// 	{
	// 		shader = componentManager->GetComponent<Shader>(shaders->at("modelShadowMapShader"));
	// 	}
	// 	else if (renderer.drawingType == 1)
	// 	{
	// 		continue; // dont draw billboard
	// 		shader = componentManager->GetComponent<Shader>(shaders->at("billboardShader"));
	// 	}
	// 	else if (renderer.drawingType == 2)
	// 	{
	// 		shader = componentManager->GetComponent<Shader>(shaders->at("animatedModelShadowMapShader"));
	// 	}
			
	// 	auto& transform = componentManager->GetComponent<Transform>(entity);

	// 	shader.use();
	// 	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	// 	if (modelArray.checkLevelOfDetail == 0)
	// 	{
	// 		renderer.DrawToShadowMap(&shader,
	// 	              &modelArray.zeroLevelModel,
	// 	              transform.position,
	// 	              transform.rotation,
	// 	              transform.scale,
	// 				  dt);
	// 	}
	// 	else
	// 	{
	// 		// Gets distance between camera and model
	// 		int checkedLevelOfDetail = CheckDistance(entity, componentManager);

	// 		if (checkedLevelOfDetail == 1)	// FirstLevel of detail
	// 		{
	// 			renderer.DrawToShadowMap(&shader,
	// 	              &modelArray.firstLevelModel,
	// 	              transform.position,
	// 	              transform.rotation,
	// 	              transform.scale,
	// 				  dt);
	// 		} 
	// 		else if (checkedLevelOfDetail == 2)	// Second level of detail
	// 		{
	// 			renderer.DrawToShadowMap(&shader,
	// 	              &modelArray.secondLevelModel,
	// 	              transform.position,
	// 	              transform.rotation,
	// 	              transform.scale,
	// 				  dt);
	// 		}
	// 		else if (checkedLevelOfDetail == 3)	// Third level of detail
	// 		{
	// 			renderer.DrawToShadowMap(&shader,
	// 	              &modelArray.thirdLevelModel,
	// 	              transform.position,
	// 	              transform.rotation,
	// 	              transform.scale,
	// 				  dt);
	// 		}
	// 		else	// Model outside of the view
	// 		{
	// 			// What do you expect should be here huh?
	// 		}			
	// 	}		
	// }


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window->GetWindowWidth(), window->GetWindowHeight());
 
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, texture);

	for (auto const& entity : entities) 
	{
		auto& renderer = componentManager->GetComponent<Renderer>(entity);
		auto& modelArray = componentManager->GetComponent<ModelArray>(entity);
		auto& shader = Shader();
		if (renderer.drawingType == 0)
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
		}
		else if (renderer.drawingType == 3)
		{
			shader = componentManager->GetComponent<Shader>(shaders->at("cubemapShader"));
			shader.use();
			shader.setInt("skybox", cubemapTexture);
			shader.setVec3("cameraPos", componentManager->GetComponent<Camera>(cameraEntity).cameraPos);
		}
			
		auto& transform = componentManager->GetComponent<Transform>(entity);

		
		// shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		// shader.setInt("shadowMap", 0);

		if (modelArray.checkLevelOfDetail == 0)
		{
			renderer.Draw(&shader,
		              &modelArray.zeroLevelModel,
		              &componentManager->GetComponent<Camera>(cameraEntity),
		              transform.position,
		              transform.rotation,
		              transform.scale,
		              this->window->GetWindowWidth(),
		              this->window->GetWindowHeight(),
					  dt);
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
		              this->window->GetWindowHeight(),
					  dt);
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
		              this->window->GetWindowHeight(),
					  dt);
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
		              this->window->GetWindowHeight(),
					  dt);
			}
			else	// Model outside of the view
			{
				// What do you expect should be here huh?
			}			
		}		
	}

	auto& camera = componentManager->GetComponent<Camera>(cameraEntity);

	auto& cubemapShader = componentManager->GetComponent<Shader>(shaders->at("cubemapShader"));
	cubemapShader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 1.0f));
    glm::mat4 view =
	glm::lookAt(camera.cameraPos,
				camera.cameraPos + camera.cameraFront,
				camera.cameraUp);


    glm::mat4 projection = 
	glm::perspective(glm::radians(camera.fieldOfView),
					(float)window->GetWindowWidth() / (float)window->GetWindowHeight(),
					0.1f,
					100.0f);
	model = glm::mat4(1.0f);
    cubemapShader.setMat4("model", model);
    cubemapShader.setMat4("view", view);
    cubemapShader.setMat4("projection", projection);
    cubemapShader.setVec3("cameraPos", camera.cameraPos);
	cubemapShader.setInt("skybox", 0);
	RenderCube();


	auto& skyboxShader = componentManager->GetComponent<Shader>(shaders->at("skyBoxShader"));
	skyboxShader.use();	
	
	view =
	glm::lookAt(glm::vec3(0.0f),
				glm::vec3(0.0f) + camera.cameraFront,
				camera.cameraUp);
	skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));

	skyboxShader.setMat4("projection", projection);
	skyboxShader.setInt("skybox", 0);
	RenderSkyBox();

	
}

void
RenderSystem::Init()
{

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    glGenFramebuffers(1, &FBO);
    // create depth texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Load skybox textures
	vector<std::string> faces
    {
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg"
    };
	cubemapTexture = LoadCubemap(faces);	
	
	float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
	glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	float cubeVertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
	glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
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

void
RenderSystem::RenderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
};

void
RenderSystem::RenderSkyBox()
{
	glDepthFunc(GL_LEQUAL);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
};

void
RenderSystem::RenderCube()
{
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
};

unsigned int RenderSystem::LoadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
};

