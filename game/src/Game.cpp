#include <chrono>
#include <iostream>
#include <random>

#include <glm/glm.hpp>

#include "Game.hpp"

#include "ecs.hpp"
#include "PhysicsSystem.hpp"
#include "RenderSystem.hpp"
#include "CameraSystem.hpp"


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

	// Register the systems used during the gameplay
	auto physicsSystem = gameplayManager->RegisterSystem<PhysicsSystem>();

	auto cameraSystem = gameplayManager->RegisterSystem<CameraSystem>();

	auto renderSystem = gameplayManager->RegisterSystem<RenderSystem>();

	// Set the components required by a system
	
	//PhysicsSystem
	gameplayManager->SetRequiredComponent<PhysicsSystem>(
	  gameplayManager->GetComponentType<Gravity>());
	gameplayManager->SetRequiredComponent<PhysicsSystem>(
	  gameplayManager->GetComponentType<RigidBody>());
	gameplayManager->SetRequiredComponent<PhysicsSystem>(
	  gameplayManager->GetComponentType<Transform>());
	  
	
	//CameraSystem
	gameplayManager->SetRequiredComponent<CameraSystem>(
	 	gameplayManager->GetComponentType<Camera>());

	

	
	//RenderSystem
	gameplayManager->SetRequiredComponent<RenderSystem>(
		gameplayManager->GetComponentType<Renderer>());

	//Create cameraEntity and add Camera component to it and attach it to the RenderSystem
	Entity cameraEntity = gameplayManager->CreateEntity();
	
	gameplayManager->AddComponent(
	  	cameraEntity,
	  	Camera(glm::vec3(-0.4f, 0.0f, 1.0f),
	  	 					glm::vec3(0.0f, 0.0f, -1.0f),
	  	 					glm::vec3(0.0f, 1.0f, 0.0f),
	  	 					45.0f)
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
		  entity, Gravity{ glm::vec3(0.0f, randGravity(generator), 0.0f) });

		gameplayManager->AddComponent(
		  entity,
		  RigidBody{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) });

		gameplayManager->AddComponent(entity,
		                              Transform{ glm::vec3(randPosition(generator),
		                                                   randPosition(generator),
		                                                   randPosition(generator)),
		                                         glm::vec3(randRotation(generator),
		                                                   randRotation(generator),
		                                                   randRotation(generator)),
		                                         glm::vec3(scale, scale, scale) });

		gameplayManager->AddComponent(
		entity, Model("assets/models/Wolf_dae.dae")
		);
		gameplayManager->AddComponent(
		entity, Shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl")
		);
		gameplayManager->AddComponent(
		entity, Renderer()
		);

		
	}

	float dt = 0.0f;
	
	renderSystem->Init();

	while (!gameWindow->ShouldClose()) {
		auto startTime = std::chrono::high_resolution_clock::now();
		gameWindow->PollEvents();
		gameWindow->ProcessInput();
		// w->UpdateViewport();
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