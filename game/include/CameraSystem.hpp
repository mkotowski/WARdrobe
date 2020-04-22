#pragma once

#include "ecs.hpp"

class CameraSystem : public System
{
public:
	void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	Entity Init();
};

Entity
CameraSystem::Init()
{
	for (auto& entity : entities)
	{
		return entity;
	}
}

void
CameraSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{
	for (auto& entity : entities) {
		auto& camera = componentManager->GetComponent<Camera>(entity);
		auto& transform = componentManager->GetComponent<Transform>(entity);

		const float radius = 10.0f;
		float       camX = sin(glfwGetTime()) * radius;
		float       camZ = cos(glfwGetTime()) * radius;
		glm::mat4   view;
		view = glm::lookAt(glm::vec3(camX, 0.0, camZ),
		                   glm::vec3(0.0, 0.0, 0.0),
		                   glm::vec3(0.0, 1.0, 0.0));
		// transform.position += 0.01;
		//transform.position = glm::vec3(camX, transform.position[1], camZ);
		// transform.scale += 1;
		transform.position = glm::vec3(camX / 8, transform.position[1], camZ / 8);
		//transform.rotation =
		//  glm::vec3(camX / 8, transform.rotation[1], transform.rotation[2]);
		camera.cameraFront =
		  glm::vec3(-camX / 8, -transform.position[1], -camZ / 8);
		//camera.cameraFront = glm::vec3(
		//  camera.cameraFront[0], camera.cameraFront[1], camera.cameraFront[2]);
		//
		camera.cameraPos = transform.position;
	}
}
