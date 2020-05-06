#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "ecs.hpp"

struct BoundingBox 
{
	
	glm::vec3 origin;
	vector<Mesh> meshes;
	glm::vec3      scale;
	vector<Vertex> vertices;
	int    indexes[6];
	glm::vec3 dimension;
	bool           isVerticesAssign = false;
	float     minX, maxX, minY, maxY, minZ, maxZ;
	void      Assign(Entity e, std::shared_ptr<ComponentManager> componentManager)
	{ 
		if (!isVerticesAssign) {
			GetVertices();
			GetDimensions();
			isVerticesAssign = true;
		}
		meshes = componentManager->GetComponent<Model>(e).meshes;
		origin = componentManager->GetComponent<Transform>(e).position;
		GetVertices();
		minX = vertices[indexes[0]].Position.x + origin[0] * scale[0];
		maxX = vertices[indexes[1]].Position.x + origin[0] * scale[0];
		minY = vertices[indexes[2]].Position.y + origin[1] * scale[1];
		maxY = vertices[indexes[3]].Position.y + origin[1] * scale[1];
		minZ = vertices[indexes[4]].Position.z + origin[2] * scale[2];
		maxZ = vertices[indexes[5]].Position.z + origin[2] * scale[2];
		/*cout << minX << ", " << maxX << ", " 
			 << minY << ", " << maxY << ", " 
			 << minZ << ", " << maxZ << "\n"
			 << origin[0] << ", " << origin[1] << ", " << origin[2] << "\n";*/
	}

	void GetVertices()
	{
		vertices.clear();
		//for (int j = 0; j < meshes.size(); j++)
			//{
				for (int i = 0; i < meshes[0].vertices.size(); i++) {
					vertices.push_back(meshes[0].vertices[i]);		
				}
			//}
		    
		    //cout << "Vertices assigned";
	}

	void GetDimensions() {
		float maxX = 0, maxY = 0, maxZ = 0;
		for (int i = 0; i < vertices.size(); i++) {
			for (int j = 0; j < vertices.size(); j++) {
				if (i != j) {
					if (maxX < Distance(vertices[i], vertices[j], 'x')) {
						maxX = Distance(vertices[i], vertices[j], 'x');
						indexes[0] = WhichIsSmaller(vertices[i], vertices[j], 'x', i, j);
						indexes[1] = WhichIsBigger(vertices[i], vertices[j], 'x', i, j);
					}

					if (maxY < Distance(vertices[i], vertices[j], 'y')) {
						maxY = Distance(vertices[i], vertices[j], 'y');
						indexes[2] = WhichIsSmaller(vertices[i], vertices[j], 'y', i, j);
						indexes[3] = WhichIsBigger(vertices[i], vertices[j], 'y', i, j);
					}

					if (maxZ < Distance(vertices[i], vertices[j], 'z')) {
						maxZ = Distance(vertices[i], vertices[j], 'z');
						indexes[4] = WhichIsSmaller(vertices[i], vertices[j], 'z', i, j);
						indexes[5] = WhichIsBigger(vertices[i], vertices[j], 'z', i, j);
					}
				}
			}
		}
	}

	int WhichIsBigger(Vertex a, Vertex b, char symbol, int aI, int bI) { 
		switch (symbol) {
			case 'x':
				if (a.Position.x < b.Position.x) { return bI; } 
				else { return aI; }
				//dD
			case 'y': 
				if (a.Position.y < b.Position.y) { return bI; }
				else { return aI; }
			case 'z':
				if (a.Position.z < b.Position.z) { return bI; } 
				else { return aI; }
			default:
				break;
		}
	}

	int WhichIsSmaller(Vertex a, Vertex b, char symbol, int aI, int bI) {
		switch (symbol) {
			case 'x':
				if (a.Position.x > b.Position.x) { return bI; } 
				else { return aI; }
				// dD
			case 'y':
				if (a.Position.y > b.Position.y) { return bI; } 
				else { return aI; }
			case 'z':
				if (a.Position.z > b.Position.z) { return bI; } 
				else { return aI; }
			default:
				break;
		}
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