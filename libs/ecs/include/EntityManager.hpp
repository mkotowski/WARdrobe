#pragma once

#include <cinttypes>
#include <queue>
#include <array>

#include "ECSTypes.hpp"

class EntityManager
{
public:
	EntityManager();
	Entity CreateEntity();
	void DestroyEntity(Entity entity);
	void SetSignature(Entity entity, Signature signature);
	Signature GetSignature(Entity entity);
private:
	// Queue of unused entity IDs
	std::queue<Entity> availableIDs {};
	// Array of signatures where the index corresponds to the entity ID
	std::array<Signature, MAX_ENTITIES> signatures {};
	// Total living entities - used to keep limits on how many exist
	std::uint32_t entityCount {};
};
