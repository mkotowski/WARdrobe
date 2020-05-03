#include <chrono>
#include <iostream>
#include <random>

#include <glm/glm.hpp>

#include "Game.hpp"

#include "ecs.hpp"
#include "PhysicsSystem.hpp"
#include "RenderSystem.hpp"
#include "CameraSystem.hpp"
#include "ColliderSystem.hpp"



Game::Game(std::string windowTitle)
{
	gameWindow = new Window(windowTitle);

	gameplayManager = new GameplayManager();
}

Game::~Game() {}

void
Game::Loop()
{
	// Initialize ECS managers
	gameplayManager->Init();

	// Register the components used during the gameplay
	gameplayManager->RegisterComponent<Gravity>();
	gameplayManager->RegisterComponent<RigidBody>();
	gameplayManager->RegisterComponent<Transform>();
	gameplayManager->RegisterComponent<Model>();
	gameplayManager->RegisterComponent<Shader>();
	gameplayManager->RegisterComponent<Renderer>();
	gameplayManager->RegisterComponent<Camera>();
	gameplayManager->RegisterComponent<BoundingBox>();
	gameplayManager->RegisterComponent<Collidable>();

	// Register the systems used during the gameplay
	auto physicsSystem = gameplayManager->RegisterSystem<PhysicsSystem>();

	auto cameraSystem = gameplayManager->RegisterSystem<CameraSystem>();

	auto renderSystem = gameplayManager->RegisterSystem<RenderSystem>();

	auto colliderSystem = gameplayManager->RegisterSystem<ColliderSystem>();
	// Add reference to a window
	renderSystem->window = this->gameWindow;

	// Set the components required by a system
	
	//PhysicsSystem
	gameplayManager->SetRequiredComponent<PhysicsSystem>(
	  gameplayManager->GetComponentType<Gravity>());
	gameplayManager->SetRequiredComponent<PhysicsSystem>(
	  gameplayManager->GetComponentType<RigidBody>());
	gameplayManager->SetRequiredComponent<PhysicsSystem>(
	  gameplayManager->GetComponentType<Transform>());
	gameplayManager->SetRequiredComponent<PhysicsSystem>(
	  gameplayManager->GetComponentType<BoundingBox>());
	gameplayManager->SetRequiredComponent<PhysicsSystem>(
	  gameplayManager->GetComponentType<Collidable>());
	  
	//CameraSystem
	gameplayManager->SetRequiredComponent<CameraSystem>(
	 	gameplayManager->GetComponentType<Camera>());
	gameplayManager->SetRequiredComponent<CameraSystem>(
		gameplayManager->GetComponentType<Transform>());
	
	//RenderSystem
	gameplayManager->SetRequiredComponent<RenderSystem>(
		gameplayManager->GetComponentType<Renderer>());
	gameplayManager->SetRequiredComponent<RenderSystem>(
		gameplayManager->GetComponentType<Transform>());

	//ColliderSystem
	gameplayManager->SetRequiredComponent<ColliderSystem>(
	  gameplayManager->GetComponentType<BoundingBox>());
	gameplayManager->SetRequiredComponent<ColliderSystem>(
	  gameplayManager->GetComponentType<Collidable>());

	//Create cameraEntity and add Camera component to it and attach it to the RenderSystem
	Entity cameraEntity = gameplayManager->CreateEntity();
	
	gameplayManager->AddComponent(cameraEntity,
	                              Transform{ glm::vec3(0.0f, 0.0f, 3.0f),
	                                         glm::vec3(0.0f, 0.0f, 0.0f),
	                                         glm::vec3(1.0f, 1.0f, 1.0f) });

	gameplayManager->AddComponent(
	  	cameraEntity,
	  	Camera(glm::vec3(0.0f, 0.0f, 0.0f),
	  	 					glm::vec3(0.0f, 0.0f, -1.0f),
	  	 					glm::vec3(0.0f, 1.0f, 0.0f),
	  	 					80.0f)
		);

	renderSystem->cameraEntity = cameraEntity;

	std::vector<Entity> entities(1);	

	std::default_random_engine            generator;
	std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
	std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
	std::uniform_real_distribution<float> randScale(3.0f, 5.0f);
	std::uniform_real_distribution<float> randGravity(-10.0f, -1.0f);

	float scale = randScale(generator);

	for (auto& entity : entities) {
		entity = gameplayManager->CreateEntity();

		gameplayManager->AddComponent(
		  entity, Gravity{ glm::vec3(0.0f, 0.0f, 0.1f) });

		gameplayManager->AddComponent(
		  entity,
		  RigidBody{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) });

		gameplayManager->AddComponent(entity,
		                              Transform{ glm::vec3(0.0f, 0.0f, 0.0f),
		                                         glm::vec3(0.0f, 0.0f, 0.0f),
		                                         glm::vec3(1.0f, 1.0f, 1.0f) });

		gameplayManager->AddComponent(
			entity, 
			Model("assets/models/Wolf_dae.dae", "assets/textures/stone2.jpg")
		);
		gameplayManager->AddComponent(
			entity, 
			Shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl")
		);
		gameplayManager->AddComponent(
			entity, 
			Renderer()
		);
		gameplayManager->AddComponent(
		  entity,
		  BoundingBox{ gameplayManager->GetComponent<Transform>(entity).position,
		               gameplayManager->GetComponent<Model>(entity).meshes}
		);
		gameplayManager->AddComponent(entity, Collidable());
		std::cout << gameplayManager->GetComponent<BoundingBox>(entity).dimension[0]
		          << " "
		          << gameplayManager->GetComponent<BoundingBox>(entity).dimension[1]
		          << " "
		          << gameplayManager->GetComponent<BoundingBox>(entity).dimension[2]
		          << "\n";
	}

	Entity modelEntity = gameplayManager->CreateEntity();

	gameplayManager->AddComponent(modelEntity,
		                              Transform{ glm::vec3(0.0f, 0.0f, 0.0f),
		                                         glm::vec3(0.0f, 0.0f, 0.0f),
		                                         glm::vec3(0.1f, 0.1f, 0.1f) });

	gameplayManager->AddComponent(
		modelEntity, 
		Model("assets/models/chr_knight.obj", "assets/textures/chr_knight.png")
	);
	gameplayManager->AddComponent(
		modelEntity, 
		Shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl")
	);
	gameplayManager->AddComponent(
		modelEntity, 
		Renderer()
	);
	gameplayManager->AddComponent(
	  modelEntity,
	  BoundingBox{ gameplayManager->GetComponent<Transform>(modelEntity).position,
	               gameplayManager->GetComponent<Model>(modelEntity).meshes}
	);
	gameplayManager->AddComponent(modelEntity, Collidable());

	float dt = 0.0f;
	
	renderSystem->Init();

	while (!gameWindow->ShouldClose()) {
		auto startTime = std::chrono::high_resolution_clock::now();
		gameWindow->PollEvents();
		gameWindow->ProcessInput();
		gameWindow->UpdateViewport();
		gameWindow->ClearScreen();

		gameplayManager->Update(dt);

#if INCLUDE_DEBUG_UI
		gameWindow->RenderDebugUI();
#endif // INCLUDE_DEBUG_UI

		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime -
		                                                                startTime)
		       .count();

		gameWindow->SwapBuffers();
	}

	delete gameWindow;

	gameWindow = nullptr;
}