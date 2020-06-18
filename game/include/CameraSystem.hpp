#pragma once

#include "ecs.hpp"

#include "ActionManager.hpp"

#include "Camera.hpp"
// TODO: extract Component structs to separate headers
#include "PhysicsSystem.hpp"

#include "Constants.h"

#include <glm/gtc/matrix_transform.hpp>

float forwardInput = 0.0f;
float rightInput = 0.0f;
float playerInputHorizontal = 0.0f;
float playerInputVertical = 0.0f;
bool  leftMousePressed = false;
bool  rightMousePressed = false;

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
	const float cameraSpeed = 1.0f;

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
			std::cout << "FORWARD CAMERA VALUE: " << actionForwardMove->rawValue
			          << "\n";
		}
		if (actionRightMove != nullptr) {
			camZ = static_cast<float>(actionRightMove->GetValue());
		}

		// transform.position = glm::vec3(camX / 8, transform.position[1], camZ /
		// 8);
		/*camera.cameraFront =
		  glm::vec3(-camX / 8, -transform.position[1], -camZ / 8);*/

		//std::cout << "Camera system forward: " << forwardInput << std::endl;

		/*camera.cameraFront += glm::vec3(rightInput,0,0);
		camera.cameraFront = glm::normalize(camera.cameraFront);
		rightInput = 0.0f;*/

		//epilepsy
		/*glm::mat4 rotate(1.0f);
		glm::vec3 cross = glm::cross(camera.cameraPos - camera.cameraFront,
		                             camera.cameraUp - camera.cameraFront);

		rotate = glm::rotate(rotate, 0.5f, cross);

		camera.cameraPos = glm::vec3(rotate * glm::vec4(camera.cameraPos, 1.0f));
		camera.cameraUp = glm::vec3(rotate * glm::vec4(camera.cameraUp, 1.0f));*/

		//float camX = sin(glfwGetTime());
		//float camZ = cos(glfwGetTime());

		camera.cameraPos = transform.position;

		/*glm::mat4 rotate(1.0f);
		float     yaw = rightInput;
		rotate =
		  glm::rotate(rotate, yaw, glm::vec3(camera.cameraUp - camera.cameraFront));

		camera.cameraPos = glm::vec3(rotate * glm::vec4(camera.cameraPos, 1.0f));
		camera.cameraUp = glm::vec3(rotate * glm::vec4(camera.cameraUp, 1.0f));*/

		//camera.cameraPos += forwardInput * cameraSpeed * camera.cameraFront;
		//camera.cameraPos = glm::vec3(camera.cameraPos[0] + rightInput,
		//                             camera.cameraPos[1],
		//                             camera.cameraPos[2]); // camZ
		/*camera.cameraPos += 
		  glm::normalize(glm::cross(camera.cameraFront, camera.cameraUp)) *
		  cameraSpeed;*/
		//forwardInput = 0;
		//rightInput = 0;

		transform.position = camera.cameraPos;
		// smooth movement
		if (forwardInput > 0.0f)
			forwardInput -= 0.01f;
		if (rightInput > 0.0f)
			rightInput -= 0.01f;
		if (forwardInput < 0.0f)
			forwardInput += 0.01f;
		if (rightInput < 0.0f)
			rightInput += 0.01f;
		// poślizg
		if (abs(forwardInput) < 0.2f)
			forwardInput = 0.0f;
		if (abs(rightInput) < 0.2f)
			rightInput = 0.0f;
	}
}
