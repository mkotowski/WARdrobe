#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "ecs.hpp"

struct BoundingBox 
{
	
	glm::vec3 origin;
	vector<Mesh> meshes;
	vector<Vertex> vertices;
	glm::vec3 dimension;
	bool           isVerticesAssign = false;
	float     minX, maxX, minY, maxY, minZ, maxZ;
	void      Assign(Entity e, std::shared_ptr<ComponentManager> componentManager)
	{ 
		if (!isVerticesAssign) {
			GetVertices();
			isVerticesAssign = true;
		}
		origin = componentManager->GetComponent<Transform>(e).position;
		minX = origin[0];
		maxX = origin[0] + dimension[0];
		minY = origin[1];
		maxY = origin[1] + dimension[1];
		minZ = origin[2];
		maxZ = origin[2] + dimension[2];
		cout << "Pozycja figury:\n " << minX << ", " << maxX << ", \n" << minY << ", "
		     << maxY << ", \n" << minZ << ", " << maxZ << "\n";
		cout << "Rozmiary " << dimension[0] << ", " << dimension[1] << ", "
		     << dimension[2] << "\n";
 	}

	void GetVertices()
	{
		cout << "Vertices assigned";
		//for (int j = 0; j < meshes.size(); j++)
			//{
				for (int i = 0; i < meshes[0].vertices.size(); i++) {
					vertices.push_back(meshes[0].vertices[i]);		
				}
			//}
		GetDimensions();
	}

	void GetDimensions() {
		float maxX = 0, maxY = 0, maxZ = 0;
		for (int i = 0; i < vertices.size(); i++) {
			for (int j = 0; j < vertices.size(); j++) {
				if (i != j) {
					if (maxX < Distance(vertices[i], vertices[j], 'x')) {
						maxX = Distance(vertices[i], vertices[j], 'x');
					}

					if (maxY < Distance(vertices[i], vertices[j], 'y')) {
						maxY = Distance(vertices[i], vertices[j], 'y');
					}

					if (maxZ < Distance(vertices[i], vertices[j], 'z')) {
						maxZ = Distance(vertices[i], vertices[j], 'z');
					}
				}
			}
		}
		dimension = glm::vec3(maxX, maxY, maxZ);
	}

	float Distance(Vertex a, Vertex b, char option) {
		switch (option) {
			case 'x':
				return abs(a.Position.x - b.Position.x);
			case 'y':
				return abs(a.Position.y - b.Position.y);
			case 'z':
				return abs(a.Position.z - b.Position.z);
			default:
				return 0;
		}
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
	void Initiate(std::shared_ptr<ComponentManager> componentManager);
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

	for (int i = 0; i < entitiesToCollide.size() - 1; i++) {
		auto& bounds1 =
		  componentManager->GetComponent<BoundingBox>(entitiesToCollide[i]);
		auto& bounds2 =
		  componentManager->GetComponent<BoundingBox>(entitiesToCollide[i + 1]);

		if (bounds1.minX < bounds2.maxX && bounds1.maxX > bounds2.minX &&
		    bounds1.minY < bounds2.maxY && bounds1.maxY > bounds2.minY &&
		    bounds1.minZ < bounds2.maxZ && bounds1.maxZ > bounds2.minZ) {

			std::cout << "COLLISION DETECTED!!!!!!!!!\n";
		} else {
			std::cout << "Collision Undetected!!!\n";
		}
	}
}

void
ColliderSystem::Initiate(std::shared_ptr<ComponentManager> componentManager)
{
	for (auto const& entity : entities) {
		componentManager->GetComponent<BoundingBox>(entity).GetVertices();
	}
}