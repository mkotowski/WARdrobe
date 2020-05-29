#pragma once

#include "PhysicsSystem.hpp"

class Character
{
	float hp;

public:
	Character() {}
	Character::Character(float hp)
	  : hp(hp)
	{}
	void Move(RigidBody* rigidBody, float x, float y, float z, float speed);
};

void
Character::Move(RigidBody* rigidBody, float x, float y, float z, float speed)
{
	glm::vec3 directionWithSpeed = glm::vec3(x, y, z);
	directionWithSpeed = glm::normalize(directionWithSpeed) * speed;

	rigidBody->velocity = directionWithSpeed;
}