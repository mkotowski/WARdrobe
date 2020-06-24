#pragma once

#include "ecs.hpp"

#include "ActionManager.hpp"

#include "Camera.hpp"
// TODO: extract Component structs to separate headers
#include "PhysicsSystem.hpp"

#include "Constants.h"

#include <glm/gtc/matrix_transform.hpp>

#include "VirtualInputManager.hpp"

float forwardInput = 0.0f;
float rightInput = 0.0f;
float playerInputHorizontal = 0.0f;
float playerInputVertical = 0.0f;
bool  leftMousePressed = false;
bool  rightMousePressed = false;
bool  ePressed = false;
bool  qPressed = false;

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

	VirtualInputManager* vim = VirtualInputManager::GetInstance();

	forwardInput = vim->GetVirtualInput("MoveForward")->GetValue() -
	               vim->GetVirtualInput("MoveBackward")->GetValue();

	rightInput = vim->GetVirtualInput("MoveRight")->GetValue() -
	             vim->GetVirtualInput("MoveLeft")->GetValue();

	leftMousePressed =
	  static_cast<bool>(vim->GetVirtualInput("AttackLeftHand")->GetValue());
	rightMousePressed =
	  static_cast<bool>(vim->GetVirtualInput("AttackRightHand")->GetValue());

	ePressed = vim->GetVirtualInput("ActivateBuff")->GetValue();
	qPressed = vim->GetVirtualInput("ActivateHeal")->GetValue();

	for (auto& entity : entities) {
		auto& camera = componentManager->GetComponent<Camera>(entity);
		auto& transform = componentManager->GetComponent<Transform>(entity);

		camera.cameraPos = transform.position;

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
