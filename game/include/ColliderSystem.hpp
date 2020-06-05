#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "ecs.hpp"
#include "Window.hpp"

struct BoundingBox 
{
	float width;
	float height;
	float depth;
	std::string tag;
	glm::vec3 origin;
	glm::vec3 rotation;
	glm::vec3 scale;
	
	vector<Mesh> meshes;
	vector<Vertex> vertices;
	int    indexes[6];
	glm::vec3 dimension;
	bool           isVerticesAssign = false;
	float     minX, maxX, minY, maxY, minZ, maxZ;
	float     minXLoc, maxXLoc, minYLoc, maxYLoc, minZLoc, maxZLoc;
	glm::vec4      AABBVertices[8];
	glm::vec3      prevRotation;

	std::vector<Entity> collisionEnterEntities;

	void StartAABB() {
		minXLoc = -width / 2.0f;
		maxXLoc = width / 2.0f;
		minYLoc = -height;
		maxYLoc = 0.0f;
		minZLoc = -depth / 2.0f;
		maxZLoc = depth / 2.0f;

		AABBVertices[0] = glm::vec4(maxXLoc, maxYLoc, minZLoc, 0.0f);
		AABBVertices[1] = glm::vec4(minXLoc, maxYLoc, minZLoc, 0.0f);
		AABBVertices[2] = glm::vec4(minXLoc, minYLoc, minZLoc, 0.0f);
		AABBVertices[3] = glm::vec4(maxXLoc, minYLoc, minZLoc, 0.0f);
		AABBVertices[4] = glm::vec4(maxXLoc, minYLoc, maxZLoc, 0.0f);
		AABBVertices[5] = glm::vec4(maxXLoc, maxYLoc, maxZLoc, 0.0f);
		AABBVertices[6] = glm::vec4(minXLoc, maxYLoc, maxZLoc, 0.0f);
		AABBVertices[7] = glm::vec4(minXLoc, minYLoc, maxZLoc, 0.0f);

			/*cout << "LOCAL: minX " << minXLoc << ", maxX " << maxXLoc << ", minY " << minYLoc
		     << ", maxY " << maxYLoc << ", minZ " << minZLoc << ", maxZ " << maxZLoc
		     << "\n";*/
	}

	void      Assign(Entity e, std::shared_ptr<ComponentManager> componentManager)
	{ 
		// if (!isVerticesAssign) {
		// 	GetVertices();
		// 	GetDimensions();
		// 	isVerticesAssign = true;
		// }
		// meshes = componentManager->GetComponent<Model>(e).meshes;
		origin = componentManager->GetComponent<Transform>(e).position;
		rotation = componentManager->GetComponent<Transform>(e).rotation;
		scale = componentManager->GetComponent<Transform>(e).scale;
		
		//meshes = componentManager->GetComponent<Model>(e).meshes;
		//GetVertices();
		//GetDimensions(minX, maxX, minY, maxY, minZ, maxZ);

		 /*minX = vertices[indexes[0]].Position.x + origin[0] * scale[0];
		 maxX = vertices[indexes[1]].Position.x + origin[0] * scale[0];
		 minY = vertices[indexes[2]].Position.y + origin[1] * scale[1];
		 maxY = vertices[indexes[3]].Position.y + origin[1] * scale[1];
		 minZ = vertices[indexes[4]].Position.z + origin[2] * scale[2];
		 maxZ = vertices[indexes[5]].Position.z + origin[2] * scale[2];*/

		glm::mat4 model = glm::mat4(1.0f);

		model = glm::rotate(model,
		                    glm::radians(rotation[0] - prevRotation[0]),
		                    glm::vec3(1.0f, 0.0f, 0.0f)); // X axis
		model = glm::rotate(model,
		                    glm::radians(rotation[1] - prevRotation[1]),
		                    glm::vec3(0.0f, 1.0f, 0.0f)); // Y axis
		model = glm::rotate(model,
		                    glm::radians(rotation[2] - prevRotation[2]),
		                    glm::vec3(0.0f, 0.0f, 1.0f)); // Z axis

		prevRotation = rotation;

		for (int i = 0; i < 8; i++) {
			AABBVertices[i] = AABBVertices[i] * model;
		}

		SetNewBounds();

		minX = minXLoc + origin.x;
		maxX = maxXLoc + origin.x;
		minY = minYLoc + origin.y;
		maxY = maxYLoc + origin.y;
		minZ = minZLoc + origin.z;
		maxZ = maxZLoc + origin.z;

		/*if (e == 6)
		{
			cout << "minX " << minX << ", maxX " << maxX << ", minY " << minY
			     << ", maxY " << maxY << ", minZ " << minZ << ", maxZ " << maxZ
			     << "\n"
			     << origin[0] << ", " << origin[1] << ", " << origin[2] << "\n";
		}*/
	}

	void SetNewBounds() {
		for (int i = 0; i < 8; i++) {

			if (i == 0) {
				minXLoc = AABBVertices[i].x;
				maxXLoc = AABBVertices[i].x;
				minYLoc = AABBVertices[i].y;
				maxYLoc = AABBVertices[i].y;
				minZLoc = AABBVertices[i].z;
				maxZLoc = AABBVertices[i].z;
				continue;
			}

			if (AABBVertices[i].x < minXLoc) {
				minXLoc = AABBVertices[i].x;
			}

			if (AABBVertices[i].x > maxXLoc) {
				maxXLoc = AABBVertices[i].x;
			}

			if (AABBVertices[i].y < minYLoc) {
				minYLoc = AABBVertices[i].y;
			}

			if (AABBVertices[i].y > maxYLoc) {
				maxYLoc = AABBVertices[i].y;
			}

			if (AABBVertices[i].z < minZLoc) {
				minZLoc = AABBVertices[i].z;
			}

			if (AABBVertices[i].z > maxZLoc) {
				maxZLoc = AABBVertices[i].z;
			}
		}
	}

	void GetVertices()
	{
		vertices.clear();
		for (int j = 0; j < meshes.size(); j++) {
			for (int i = 0; i < meshes[j].vertices.size(); i++) {
				vertices.push_back(meshes[j].vertices[i]);
			}
		}
		    
		    //cout << "Vertices assigned";
	}

	void GetDimensions(float& minX,
	                   float& maxX,
	                   float& minY,
	                   float& maxY,
	                   float& minZ,
	                   float& maxZ)
	{
		/*for (int i = 0; i < vertices.size() + 1; i++) {
			for (int j = i + 1; j < vertices.size(); j++) {
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
		}*/

		for (int i = 0; i < vertices.size(); i++) {

			if (i == 0) {
				minX = vertices[i].Position.x;
				maxX = vertices[i].Position.x;
				minY = vertices[i].Position.y;
				maxY = vertices[i].Position.y;
				minZ = vertices[i].Position.z;
				maxZ = vertices[i].Position.z;
				continue;
			}

			if (vertices[i].Position.x < minX) {
				minX = vertices[i].Position.x;
			}

			if (vertices[i].Position.x > maxX) {
				maxX = vertices[i].Position.x;
			}

			if (vertices[i].Position.y < minY) {
				minY = vertices[i].Position.y;
			}

			if (vertices[i].Position.y > maxY) {
				maxY = vertices[i].Position.y;
			}

			if (vertices[i].Position.z < minZ) {
				minZ = vertices[i].Position.z;
			}

			if (vertices[i].Position.z > maxZ) {
				maxZ = vertices[i].Position.z;
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

class ColliderSystem : public System
{
public:
	std::vector<Entity> entitiesToCollide;
	Shader *ourShader;
	Window *window;
	Camera *camera;

	void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void CheckCollision(std::shared_ptr<ComponentManager> componentManager);
	void Initiate(std::shared_ptr<ComponentManager> componentManager);
	void DrawBoundingBox(BoundingBox box);
};

void
ColliderSystem::Update(float                             dt,
                       std::shared_ptr<ComponentManager> componentManager)
{
	//entitiesToCollide.clear();
	for (auto const& entity : entities) {
		componentManager->GetComponent<BoundingBox>(entity).Assign(entity, componentManager);
		entitiesToCollide.push_back(entity);
		componentManager->GetComponent<BoundingBox>(entity).origin = 
			componentManager->GetComponent<Transform>(entity).position;
		DrawBoundingBox(componentManager->GetComponent<BoundingBox>(entity));
	}
	CheckCollision(componentManager);
	entitiesToCollide.clear();
}

void
ColliderSystem::CheckCollision(
  std::shared_ptr<ComponentManager> componentManager)
{

	for (int i = 0; i < entitiesToCollide.size(); i++) {
		componentManager->GetComponent<BoundingBox>(entitiesToCollide[i])
		  .collisionEnterEntities.clear();
	}

	for (int i = 0; i < entitiesToCollide.size() - 1; i++) {
		auto& bounds1 =
		  componentManager->GetComponent<BoundingBox>(entitiesToCollide[i]);

		for (int j = i + 1; j < entitiesToCollide.size(); j++)
		{
			auto& bounds2 =
			  componentManager->GetComponent<BoundingBox>(entitiesToCollide[j]);

			if (bounds1.minX < bounds2.maxX && bounds1.maxX > bounds2.minX &&
			    bounds1.minY < bounds2.maxY && bounds1.maxY > bounds2.minY &&
			    bounds1.minZ < bounds2.maxZ && bounds1.maxZ > bounds2.minZ) {

				bounds1.collisionEnterEntities.push_back(entitiesToCollide[j]);
				bounds2.collisionEnterEntities.push_back(entitiesToCollide[i]);

				//std::cout << "COLLISION DETECTED!!!!!!!!!\n";

				float xDistance = bounds1.origin.x - bounds2.origin.x;
				float yDistance = bounds1.origin.y - bounds2.origin.y;
				float zDistance = bounds1.origin.z - bounds2.origin.z;

				float xGoalDistance = (bounds1.width / 2.0f) + (bounds2.width / 2.0f);
				float yGoalDistance = (bounds1.height / 2.0f) + (bounds2.height / 2.0f);
				float zGoalDistance = (bounds1.depth / 2.0f) + (bounds2.depth / 2.0f);

				float xMove = abs(xGoalDistance) - abs(xDistance);
				float yMove = abs(yGoalDistance) - abs(yDistance);
				float zMove = abs(zGoalDistance) - abs(zDistance);

				if (abs(xMove) < abs(yMove) && abs(xMove) < abs(zMove)) {
					yMove = 0.0f;
					zMove = 0.0f;

					if (xDistance < 0.0f) {
						xMove = -xMove;
					}
				} else if (abs(yMove) < abs(xMove) && abs(yMove) < abs(zMove)) {
					xMove = 0.0f;
					zMove = 0.0f;

					if (yDistance < 0.0f) {
						yMove = -yMove;
					}
				} else if (abs(zMove) < abs(xMove) && abs(zMove) < abs(yMove)) {
					xMove = 0.0f;
					yMove = 0.0f;

					if (zDistance < 0.0f) {
						zMove = -zMove;
					}
				}

				componentManager->GetComponent<Transform>(entitiesToCollide[i])
				  .position += glm::vec3(xMove, yMove, zMove);
				//componentManager->GetComponent<Transform>(entitiesToCollide[j])
				  //.position -= glm::vec3(xMove, yMove, zMove) / 2.0f;
			}
		}
	}
}

void
ColliderSystem::Initiate(std::shared_ptr<ComponentManager> componentManager)
{
	for (auto const& entity : entities) {
		componentManager->GetComponent<BoundingBox>(entity).StartAABB();
	}
}

void
ColliderSystem::DrawBoundingBox(BoundingBox box)
{	
	float vertices[] = 
	{
		(-box.width / 2.0f), box.height,  (-box.depth / 2.0f),  0.0f, 0.0f,
        (box.width / 2.0f),	  box.height, (-box.depth / 2.0f),  1.0f, 0.0f,
        (box.width / 2.0f),	  0.0f,  (-box.depth / 2.0f),  1.0f, 1.0f,
        (box.width / 2.0f),	  0.0f,  (-box.depth / 2.0f),  1.0f, 1.0f,
        (-box.width / 2.0f),  0.0f,  (-box.depth / 2.0f),  0.0f, 1.0f,
       	(-box.width / 2.0f), box.height, (-box.depth / 2.0f),  0.0f, 0.0f,

        (-box.width / 2.0f),  box.height,  (box.depth / 2.0f),  0.0f, 0.0f,
        (box.width / 2.0f),	  box.height,  (box.depth / 2.0f),  1.0f, 0.0f,
        (box.width / 2.0f),	  0.0f,  (box.depth / 2.0f),  1.0f, 1.0f,
        (box.width / 2.0f),	  0.0f,  (box.depth / 2.0f),  1.0f, 1.0f,
        (-box.width / 2.0f),  0.0f,  (box.depth / 2.0f),  0.0f, 1.0f,
        (-box.width / 2.0f),  box.height,  (box.depth / 2.0f),  0.0f, 0.0f,

        (-box.width / 2.0f),  0.0f,  (box.depth / 2.0f),  1.0f, 0.0f,
        (-box.width / 2.0f),  0.0f,  (-box.depth / 2.0f),  1.0f, 1.0f,
        (-box.width / 2.0f),  box.height, (-box.depth / 2.0f),  0.0f, 1.0f,
        (-box.width / 2.0f),  box.height, (-box.depth / 2.0f),  0.0f, 1.0f,
        (-box.width / 2.0f),  box.height, (box.depth / 2.0f),  0.0f, 0.0f,
        (-box.width / 2.0f),  0.0f,  (box.depth / 2.0f),  1.0f, 0.0f,

        (box.width / 2.0f),	  0.0f,  (box.depth / 2.0f),  1.0f, 0.0f,
        (box.width / 2.0f),	  0.0f,  (-box.depth / 2.0f),  1.0f, 1.0f,
        (box.width / 2.0f),	  box.height, (-box.depth / 2.0f),  0.0f, 1.0f,
        (box.width / 2.0f),	  box.height, (-box.depth / 2.0f),  0.0f, 1.0f,
        (box.width / 2.0f),	  box.height, (box.depth / 2.0f),  0.0f, 0.0f,
        (box.width / 2.0f),	  0.0f,  (box.depth / 2.0f),  1.0f, 0.0f,

        (-box.width / 2.0f),  box.height, (-box.depth / 2.0f),  0.0f, 1.0f,
        (box.width / 2.0f),	  box.height, (-box.depth / 2.0f),  1.0f, 1.0f,
        (box.width / 2.0f),	  box.height, (box.depth / 2.0f),  1.0f, 0.0f,
        (box.width / 2.0f),	  box.height, (box.depth / 2.0f),  1.0f, 0.0f,
        (-box.width / 2.0f),  box.height, (box.depth / 2.0f),  0.0f, 0.0f,
        (-box.width / 2.0f),  box.height , (-box.depth / 2.0f),  0.0f, 1.0f,

        (-box.width / 2.0f),  0.0f, (-box.depth / 2.0f),  0.0f, 1.0f,
        (box.width / 2.0f),	  0.0f, (-box.depth / 2.0f),  1.0f, 1.0f,
        (box.width / 2.0f),	  0.0f, (box.depth / 2.0f),  1.0f, 0.0f,
        (box.width / 2.0f),	  0.0f, (box.depth / 2.0f),  1.0f, 0.0f,
        (-box.width / 2.0f),  0.0f, (box.depth / 2.0f),  0.0f, 0.0f,
        (-box.width / 2.0f),  0.0f, (-box.depth / 2.0f),  0.0f, 1.0f
	};

	unsigned int VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	// miejsce na shader
	ourShader->use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(box.origin.x, box.origin.y, box.origin.z));
	
	model = glm::rotate(
	  model, glm::radians(box.rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f)); // X axis
	model = glm::rotate(
	  model, glm::radians(box.rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)); // Y axis
	model = glm::rotate(
	  model, glm::radians(box.rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)); // Z axis

	//model = glm::scale(model, box.scale);
	
	glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 projection = glm::mat4(1.0f);
	//https://community.khronos.org/t/application-crashes-with-the-window-resize-event/72684
	  /*
	  It’s complaining that you’re passing zero for the “aspect” parameter of glm::perspective.

		This will occur if winWidth and winHeight are both integers 
		and winWidth is less than winHeight. If you want the ratio as 
		a floating-point value, you have to convert at least one of them 
		to floating point prior to division. Your existing code divides 
		to integers which produces an integer result (in this case, zero),
		which you then convert to floating point.*/
	  if (window->GetWindowWidth() > 0 && window->GetWindowHeight() > 0)
		  {
	projection = glm::perspective(glm::radians(camera->fieldOfView),
	                              (float)window->GetWindowWidth() /
	                                (float)window->GetWindowHeight(),
	                              0.1f,
	                              100.0f);
	  } else {
		  projection = glm::perspective(glm::radians(camera->fieldOfView),
		                                1.0f /
		                                  1.0f,
		                                0.1f,
		                                100.0f);
	  }
    view = glm::lookAt(camera->cameraPos,
	              camera->cameraPos + camera->cameraFront,
	              camera->cameraUp);
    // pass transformation matrices to the shader
    ourShader->setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
    ourShader->setMat4("view", view);
	ourShader->setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}