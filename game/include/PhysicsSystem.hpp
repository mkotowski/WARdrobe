#pragma once

#include <glm/glm.hpp>
#include <iostream>

#include "ecs.hpp"

struct Gravity
{
	glm::vec3 force;
};

struct RigidBody
{
	glm::vec3 velocity;
	glm::vec3 acceleration;
};

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class PhysicsSystem : public System
{
public:
	void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
};

void
PhysicsSystem::Update(float                             dt,
                      std::shared_ptr<ComponentManager> componentManager)
{
	for (auto const& entity : entities) {
		auto&       rigidBody = componentManager->GetComponent<RigidBody>(entity);
		auto&       transform = componentManager->GetComponent<Transform>(entity);
		auto const& gravity = componentManager->GetComponent<Gravity>(entity);

		rigidBody.velocity += gravity.force * dt;

		transform.position += rigidBody.velocity * dt;
	}

	//std::cout << "Physics updated\n";
}