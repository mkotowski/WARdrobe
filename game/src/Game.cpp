#include <chrono>
#include <iostream>
#include <random>

#include <glm/glm.hpp>

#include "Game.hpp"

#include "CameraSystem.hpp"
#include "PhysicsSystem.hpp"
#include "RenderSystem.hpp"

#include "CameraSystem.hpp"
#include "ColliderSystem.hpp"
#include "ShaderSystem.hpp"

#include "ecs.hpp"


#include "ActionManager.hpp"

Game::Game(std::string windowTitle)
{
	gameWindow = new Window(windowTitle);

	gameplayManager = new GameplayManager();
}

Game::~Game() {}

void
Game::Loop()
{
	/*auto cameraForwardAction =
	  ActionManager::GetInstance().AddAction("CameraForward", ActionType::State);

	auto cameraBackAction =
	  ActionManager::GetInstance().AddAction("CameraBack", ActionType::State);

	auto cameraRightAction =
	  ActionManager::GetInstance().AddAction("CameraRight", ActionType::State);

	auto cameraLeftAction =
	  ActionManager::GetInstance().AddAction("CameraLeft", ActionType::State);

	CallbackPointer forwardPtr = std::make_shared<Callback>(
	  std::bind(&Action::execute, cameraForwardAction, std::placeholders::_1));

	CallbackPointer backPtr = std::make_shared<Callback>(
	  std::bind(&Action::execute, cameraBackAction, std::placeholders::_1));

	CallbackPointer rightPtr = std::make_shared<Callback>(
	  std::bind(&Action::execute, cameraRightAction, std::placeholders::_1));

	CallbackPointer leftPtr = std::make_shared<Callback>(
	  std::bind(&Action::execute, cameraLeftAction, std::placeholders::_1));

	gameWindow->GetInputManager()->BindAction(
	  GLFW_KEY_W, InputSource::KEY, GLFW_PRESS, 0, forwardPtr);

	gameWindow->GetInputManager()->BindAction(
	  GLFW_KEY_S, InputSource::KEY, GLFW_PRESS, 0, backPtr);

	gameWindow->GetInputManager()->BindAction(
	  GLFW_KEY_A, InputSource::KEY, GLFW_PRESS, 0, leftPtr);

	gameWindow->GetInputManager()->BindAction(
	  GLFW_KEY_D, InputSource::KEY, GLFW_PRESS, 0, rightPtr);*/

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

	auto shaderSystem = gameplayManager->RegisterSystem<ShaderSystem>();

	auto renderSystem = gameplayManager->RegisterSystem<RenderSystem>();

	auto colliderSystem = gameplayManager->RegisterSystem<ColliderSystem>();
	// Add reference to a window
	renderSystem->window = this->gameWindow;

	// Set the components required by a system

	// PhysicsSystem
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
	  
	// ColliderSystem
	gameplayManager->SetRequiredComponent<ColliderSystem>(
	  gameplayManager->GetComponentType<BoundingBox>());
	gameplayManager->SetRequiredComponent<ColliderSystem>(
	  gameplayManager->GetComponentType<Transform>());

	// CameraSystem
	gameplayManager->SetRequiredComponent<CameraSystem>(
	  gameplayManager->GetComponentType<Camera>());
	gameplayManager->SetRequiredComponent<CameraSystem>(
		gameplayManager->GetComponentType<Transform>());

	//ShaderSystem
	gameplayManager->SetRequiredComponent<ShaderSystem>(
	 	gameplayManager->GetComponentType<Shader>());
	
	//RenderSystem
	gameplayManager->SetRequiredComponent<RenderSystem>(
	  gameplayManager->GetComponentType<Renderer>());
	gameplayManager->SetRequiredComponent<RenderSystem>(
	  gameplayManager->GetComponentType<Transform>());

	// Load levelData from JSON file
	LoadLevel("assets/levels/levelTest.json");
	
	/*
		gameplayManager->AddComponent(
		modelEntity,
		BoundingBox{ gameplayManager->GetComponent<Transform>(modelEntity).position,
					gameplayManager->GetComponent<Model>(modelEntity).meshes}
		);
		gameplayManager->AddComponent(modelEntity, Collidable());
	*/
	float dt = 0.0f;
	
	// Initialize CameraSystem and bound mainCamera to RenderSystem
	cameraSystem->Init();
	renderSystem->cameraEntity = cameraSystem->cameraEntity;

	// Initialize ShaderSystem and bound map of shaders to RenderSystem
	shaderSystem->Init(gameplayManager->GetComponentManager());
	renderSystem->shaders = shaderSystem->shaders;

	colliderSystem->Initiate(gameplayManager->GetComponentManager());
	
	colliderSystem->window = this->gameWindow;
	colliderSystem->camera = &gameplayManager->GetComponentManager()->GetComponent<Camera>(cameraSystem->cameraEntity);

	colliderSystem->ourShader = &gameplayManager->GetComponentManager()->GetComponent<Shader>(shaderSystem->shaders.at("boxShader"));


	renderSystem->Init();
	
	while (!gameWindow->ShouldClose()) {
		auto startTime = std::chrono::high_resolution_clock::now();
		gameWindow->PollEvents();
		gameWindow->ProcessInput();
		gameWindow->GetInputManager()->Call();
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

void
Game::LoadLevel(std::string levelPath)
{
	// Read raw data from JSON file
	std::ifstream rawLevelData(levelPath);
	// Write that data to JSON object
	nlohmann::json jsonLevelData;
	rawLevelData >> jsonLevelData;

	for (auto& it : jsonLevelData.items()) {
		Entity entity = gameplayManager->CreateEntity();

		for (auto& it2 : it.value().items()) {
			if (it2.key() == "Model") {
				// 0: model Path
				// 1: texture Path
				gameplayManager->AddComponent(entity,
				                              Model(it2.value()[0], it2.value()[1]));
			} else if (it2.key() == "Shader") {
				// 0: vertex Path
				// 1: fragment Path
				// 2. shader type
				std::string vertexShaderPath = it2.value()[0];
				std::string fragmentShaderPath = it2.value()[1];
				std::string shaderType = it2.value()[2];
				gameplayManager->AddComponent(
											entity,
											Shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), shaderType));
			}
			else if (it2.key() == "Transform")
			{
				// 0 - 2: Position X Y Z
				// 3 - 5: Rotation X Y Z
				// 6 - 8: Scale X Y Z
				gameplayManager->AddComponent(
				  entity,
				  Transform{
				    glm::vec3(it2.value()[0], it2.value()[1], it2.value()[2]),
				    glm::vec3(it2.value()[3], it2.value()[4], it2.value()[5]),
				    glm::vec3(it2.value()[6], it2.value()[7], it2.value()[8]) });
			} else if (it2.key() == "Rigidbody") {
				// 0 - 2: Velocity X Y Z
				// 3 - 5: Acceleration X Y Z
				gameplayManager->AddComponent(
				  entity,
				  RigidBody{
				    glm::vec3(it2.value()[0], it2.value()[1], it2.value()[2]),
				    glm::vec3(it2.value()[3], it2.value()[4], it2.value()[5]) });

			} else if (it2.key() == "Gravity") {
				// 0 - 2: Gravity X Y Z
				gameplayManager->AddComponent(
				  entity,
				  Gravity{ glm::vec3(it2.value()[0], it2.value()[1], it2.value()[2]) });
			} else if (it2.key() == "Renderer") {
				// Render Component
				gameplayManager->AddComponent(entity, Renderer(it2.value()));

			} else if (it2.key() == "Camera") {
				// 0 - 2: Camera Position
				// 3 - 5: Camera Front/Target
				// 6 - 8: Camera Up
				// 9: Field Of View

				gameplayManager->AddComponent(
				  entity,
				  Camera(glm::vec3(it2.value()[0], it2.value()[1], it2.value()[2]),
				         glm::vec3(it2.value()[3], it2.value()[4], it2.value()[5]),
				         glm::vec3(it2.value()[6], it2.value()[7], it2.value()[8]),
				         it2.value()[9]));
			}
			else if (it2.key() == "BoundingBox") 
			{
				// 0 - width
				// 1 - height
				// 2 - depth
				gameplayManager->AddComponent(
					entity,
					BoundingBox{ it2.value()[0], it2.value()[1], it2.value()[2]});
			
			}
		}
	}
}