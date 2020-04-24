#pragma once

#include "ecs.hpp"

#include "ActionManager.hpp"

#include "Camera.hpp"
// TODO: extract Component structs to separate headers
#include "PhysicsSystem.hpp"

class CameraSystem : public System
{
public:
	void   Update(float                             dt,
	              std::shared_ptr<ComponentManager> componentManager) override;
	void   Init();
	Entity cameraEntity;
};

void
CameraSystem::Init()
{
	for (auto& entity : entities) {
		// Saves entity with Camera Component, right now it works as last one loaded
		this->cameraEntity = entity;
	}
}

void
CameraSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{
	const float cameraSpeed = 1.5f;

	for (auto& entity : entities) {
		auto& camera = componentManager->GetComponent<Camera>(entity);
		auto& transform = componentManager->GetComponent<Transform>(entity);

		/*const float radius = 10.0f;
		float       camX = sin(glfwGetTime()) * radius;
		float       camZ = cos(glfwGetTime()) * radius;
		glm::mat4   view;
		view = glm::lookAt(glm::vec3(camX, 0.0, camZ),
		                   glm::vec3(0.0, 0.0, 0.0),
		                   glm::vec3(0.0, 1.0, 0.0));*/

		auto actionForwardMove =
		  ActionManager::GetInstance().GetAction("CameraForward");

		auto actionBackMove =
		  ActionManager::GetInstance().GetAction("CameraBack");

		auto actionRightMove =
		  ActionManager::GetInstance().GetAction("CameraRight");

		auto actionLeftMove =
		  ActionManager::GetInstance().GetAction("CameraLeft");

		float camX = 1;
		float camZ = 1;

		if (actionForwardMove != nullptr) {
			camX = actionForwardMove->GetValue();
			std::cout << "FORWARD CAMERA VALUE: " << camX << "\n";
		}
		if (actionRightMove != nullptr) {
			camZ = static_cast<float>(actionRightMove->GetValue());
		}

		// transform.position = glm::vec3(camX / 8, transform.position[1], camZ /
		// 8);
		/*camera.cameraFront =
		  glm::vec3(-camX / 8, -transform.position[1], -camZ / 8);*/

		camera.cameraPos = transform.position;
		camera.cameraPos += camZ * cameraSpeed * camera.cameraFront;
	}
}
