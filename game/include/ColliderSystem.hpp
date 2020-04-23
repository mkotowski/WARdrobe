#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "ecs.hpp"

struct BoundingBox 
{
	glm::vec3 origin;
	glm::vec3 dimension;
};

struct Collidable
{
	bool canCollide = true;
};

class ColliderSystem : public System
{
public:
	std::vector<Entity> entitiesToCollide;
	void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void CheckCollision(std::shared_ptr<ComponentManager> componentManager);
};

void
ColliderSystem::Update(float                             dt,
                       std::shared_ptr<ComponentManager> componentManager)
{
	entitiesToCollide.clear();
	for (auto const& entity : entities) {
		entitiesToCollide.push_back(entity);
	}
	CheckCollision(componentManager);
}

void
ColliderSystem::CheckCollision(
  std::shared_ptr<ComponentManager> componentManager)
{
	//if (entitiesToCollide.size() > 1) {

		for (int i = 0; i < entitiesToCollide.size() - 1; i++) {
			auto& bounds1 =
			  componentManager->GetComponent<BoundingBox>(entitiesToCollide[i]);
			auto& bounds2 =
			  componentManager->GetComponent<BoundingBox>(entitiesToCollide[i + 1]);
			if (bounds1.origin[0] < bounds2.origin[0] + bounds2.dimension[0] &&
			    bounds1.origin[0] + bounds1.dimension[0] > bounds2.origin[0] &&
			    bounds1.origin[1] < bounds2.origin[1] + bounds2.dimension[1] &&
			    bounds1.origin[1] + bounds1.dimension[1] > bounds2.origin[1] &&
			    bounds1.origin[2] < bounds2.origin[2] + bounds2.dimension[2] &&
			    bounds1.origin[2] + bounds1.dimension[2] > bounds2.origin[2]) {
				std::cout << "COLLISION DETECTED!!!!!!!!!\n";
			} 
			else	
			{
			  std::cout << "Collision Undetected!!!\n";  
			}
		}
	//}
}