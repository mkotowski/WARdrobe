#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "ecs.hpp"

struct BoundingBox 
{
	
	glm::vec3 origin;
	glm::vec3 dimension;
	float     minX, maxX, minY, maxY, minZ, maxZ;
	void      Assign(Entity e, std::shared_ptr<ComponentManager> componentManager)
	{ 
		origin = componentManager->GetComponent<Transform>(e).position;
		minX = origin[0];
		maxX = origin[0] + dimension[0];
		minY = origin[1];
		maxY = origin[1] + dimension[1];
		minZ = origin[2];
		maxZ = origin[2] + dimension[2];
	}
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
		componentManager->GetComponent<BoundingBox>(entity).Assign(entity, componentManager);
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

			std::cout << bounds1.minX << "   " << bounds1.maxX << "\n";
		  std::cout << bounds1.minY << "   " << bounds1.maxY << "\n";
		  std::cout << bounds1.minZ << "   " << bounds1.maxZ << "\n";
		  std::cout << bounds2.minX << "   " << bounds2.maxX << "\n";
		  std::cout << bounds2.minY << "   " << bounds2.maxY << "\n";
		  std::cout << bounds2.minZ << "   " << bounds2.maxZ << "\n"; 

			if (bounds1.minX < bounds2.maxX &&
			    bounds1.maxX > bounds2.minX &&
		        bounds1.minY < bounds2.maxY && 
				bounds1.maxY > bounds2.minY &&
		        bounds1.minZ < bounds2.maxZ && 
				bounds1.maxZ > bounds2.minZ) {
			  
				std::cout << "COLLISION DETECTED!!!!!!!!!\n";
			} 
			else	
			{
			  std::cout << "Collision Undetected!!!\n";  
			}
		}
	//}
}