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

#include "LightningSystem.hpp"
#include "AnimationSystem.hpp"
#include "ScriptsSystem.hpp"


#include "ecs.hpp"


#include "ActionManager.hpp"

Game::Game(std::string windowTitle)
{
	gameWindow = new Window(windowTitle);

	gameWindow->GetDebugUI()->AddLog("test imgui logger\n");

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
	gameplayManager->RegisterComponent<Shader>();
	gameplayManager->RegisterComponent<Renderer>();
	gameplayManager->RegisterComponent<Camera>();
	gameplayManager->RegisterComponent<Light>();
	gameplayManager->RegisterComponent<Animator>();
	gameplayManager->RegisterComponent<BoundingBox>();
	gameplayManager->RegisterComponent<Scripts>();
	gameplayManager->RegisterComponent<ModelArray>();

	// Register the systems used during the gameplay
	auto physicsSystem = gameplayManager->RegisterSystem<PhysicsSystem>();

	auto cameraSystem = gameplayManager->RegisterSystem<CameraSystem>();

	auto shaderSystem = gameplayManager->RegisterSystem<ShaderSystem>();

	auto lightningSystem = gameplayManager->RegisterSystem<LightningSystem>();

	auto renderSystem = gameplayManager->RegisterSystem<RenderSystem>();

	auto animationSystem = gameplayManager->RegisterSystem<AnimationSystem>();

	auto colliderSystem = gameplayManager->RegisterSystem<ColliderSystem>();

	auto scriptsSystem = gameplayManager->RegisterSystem<ScriptsSystem>();
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
	gameplayManager->SetRequiredComponent<ModelArray>(
	   gameplayManager->GetComponentType<Renderer>());
	gameplayManager->SetRequiredComponent<RenderSystem>(
	  gameplayManager->GetComponentType<Transform>());

	// LightningSystem
	gameplayManager->SetRequiredComponent<LightningSystem>(
	  gameplayManager->GetComponentType<Light>());

	// AnimationSystem
	gameplayManager->SetRequiredComponent<AnimationSystem>(
	   gameplayManager->GetComponentType<ModelArray>());
	gameplayManager->SetRequiredComponent<AnimationSystem>(
		gameplayManager->GetComponentType<Animator>());
	// ScriptsSystem
	gameplayManager->SetRequiredComponent<ScriptsSystem>(
	  gameplayManager->GetComponentType<Scripts>());

	// Load levelData from JSON file
	LoadLevel("assets/levels/levelTest.json");
	std::cout << "Level has been loaded" << std::endl;
	float dt = 0.0f;
	
	// Initialize CameraSystem and bound mainCamera to RenderSystem
	cameraSystem->Init();
	renderSystem->cameraEntity = cameraSystem->cameraEntity;
	std::cout << "CameraSystem has been initialized" << std::endl;

	// Initialize LightningSystem
	lightningSystem->Init(gameplayManager->GetComponentManager());
	std::cout << "LightningSystem has been initialized" << std::endl;
	// Bind light to RenderSystem
	renderSystem->lightEntity = lightningSystem->dirLight;

	// Initialize ShaderSystem and bound map of shaders to RenderSystem and LightningSystem
	shaderSystem->Init(gameplayManager->GetComponentManager());
	std::cout << "ShaderSystem has been initialized" << std::endl;

	shaderSystem->cameraEntity = cameraSystem->cameraEntity;
	renderSystem->shaders = &shaderSystem->shaders;
	lightningSystem->shaders = &shaderSystem->shaders;
	
	std::cout << "Here" << std::endl;	
	animationSystem->Init(gameplayManager->GetComponentManager());
	std::cout << "AnimationSystem has been initialized" << std::endl;	
	
	colliderSystem->window = this->gameWindow;
	colliderSystem->camera = &gameplayManager->GetComponentManager()->GetComponent<Camera>(cameraSystem->cameraEntity);
	
	colliderSystem->ourShader = &gameplayManager->GetComponentManager()->GetComponent<Shader>(shaderSystem->shaders.at("boxShader"));
	
	colliderSystem->Initiate(gameplayManager->GetComponentManager());
	std::cout << "ColliderSystem has been initialized" << std::endl;

	renderSystem->Init();
	std::cout << "RenderSystem has been initialized" << std::endl;

	scriptsSystem->Init(gameplayManager->GetComponentManager());

	gameWindow->CloseSplashScreen();
	// avoid displaying empty window
	gameWindow->ShowWindow();
	
	while (!gameWindow->ShouldClose()) {
		auto startTime = std::chrono::high_resolution_clock::now();
		gameWindow->PollEvents();
		gameWindow->ProcessInput();
		gameWindow->GetInputManager()->Call();
		gameWindow->UpdateViewport();
		gameWindow->ClearScreen();
		gameplayManager->Update(dt);
		gameWindow->TestGUI();
#if INCLUDE_DEBUG_UI
		gameWindow->RenderDebugUI();
#endif // INCLUDE_DEBUG_UI

		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime -
		                                                                startTime)
		       .count();
		
		gameWindow->SwapBuffers();
	}

	scriptsSystem->CloseLuaState(gameplayManager->GetComponentManager());

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
			if (it2.key() == "ModelArray") 
			{
				gameplayManager->AddComponent(entity,
											  ModelArray(it2.value()[0]));
				if (it2.value()[0] == 0)
				{
					// 0: - chechLevelOfDetail (false)
					// 1: - model Path
					// 2: - texture Path
					gameplayManager->GetComponent<ModelArray>(entity).zeroLevelModel = Model(it2.value()[1], it2.value()[2]);
					
				}
				else
				{
					// 0: 		chechLevelOfDetail (true)
					// 1/3/5: - modelPath   (first/second/third LoD)
					// 2/4/6: - texturePath (first/second/third LoD)
					gameplayManager->GetComponent<ModelArray>(entity).firstLevelModel = Model(it2.value()[1], it2.value()[2]);
					gameplayManager->GetComponent<ModelArray>(entity).secondLevelModel = Model(it2.value()[3], it2.value()[4]);
					gameplayManager->GetComponent<ModelArray>(entity).thirdLevelModel = Model(it2.value()[5], it2.value()[6]);
						
				}
			} 
			else if (it2.key() == "Shader") 
			{
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
			} 
			else if (it2.key() == "Rigidbody") 
			{
				// 0 - 2: Velocity X Y Z
				// 3 - 5: Acceleration X Y Z
				gameplayManager->AddComponent(
				  entity,
				  RigidBody{
				    glm::vec3(it2.value()[0], it2.value()[1], it2.value()[2]),
				    glm::vec3(it2.value()[3], it2.value()[4], it2.value()[5]) });

			} 
			else if (it2.key() == "Gravity") 
			{
				// 0 - 2: Gravity X Y Z
				gameplayManager->AddComponent(
				  entity,
				  Gravity{ glm::vec3(it2.value()[0], it2.value()[1], it2.value()[2]) });
			} 
			else if (it2.key() == "Renderer") 
			{
				// Render Component
				gameplayManager->AddComponent(entity, Renderer(it2.value()));

			} 
			else if (it2.key() == "Camera") 
			{
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
			} else if (it2.key() == "BoundingBox") {
				// 0 - width
				// 1 - height
				// 2 - depth
				gameplayManager->AddComponent(
					entity,
					BoundingBox{ it2.value()[0], it2.value()[1], it2.value()[2]});
			
			} else if (it2.key() == "Scripts") {
				std::list<std::string> scripts;

				for (auto script : it2.value()) {
					scripts.push_back(script);
				}

				gameplayManager->AddComponent(entity, Scripts{ scripts });
			}
			else if (it2.key() == "Light")
			{
				// 0 - type of the light
				// Directional Light
				if (it2.value()[0] == "directionalLight")
				{
					// 1  - 3:  - direction
					// 4  - 6:  - ambient
					// 7  - 9:  - diffuse
					// 10 - 12: - specular
					gameplayManager->AddComponent(
						entity,
						Light(glm::vec3(it2.value()[1], it2.value()[2], it2.value()[3]),
							  glm::vec3(it2.value()[4], it2.value()[5], it2.value()[6]),
							  glm::vec3(it2.value()[7], it2.value()[8], it2.value()[9]),
							  glm::vec3(it2.value()[10], it2.value()[11], it2.value()[12]),
							  glm::vec3(it2.value()[13], it2.value()[14], it2.value()[15]))
					);
				}
				else if(it2.value()[0] == "pointLight")
				{
					// 1  - 3:  - position
					// 4 		- constant
					// 5 		- linear
					// 6 		- quadratic
					// 7  - 9:  - ambient
					// 10 - 12: - diffuse
					// 13 - 15: - specular
					gameplayManager->AddComponent(
						entity,
						Light(glm::vec3(it2.value()[1], it2.value()[2], it2.value()[3]),
							  it2.value()[4],
							  it2.value()[5],
							  it2.value()[6],
							  glm::vec3(it2.value()[7], it2.value()[8], it2.value()[9]),
							  glm::vec3(it2.value()[10], it2.value()[11], it2.value()[12]),
							  glm::vec3(it2.value()[13], it2.value()[14], it2.value()[15]))
					);
				}
				else if (it2.value()[0] == "spotLight")
				{
					// 1  - 3:  - direction
					// 4  - 6:  - position
					// 7 		- constant
					// 8 		- linear
					// 9 		- quadratic
					// 10 - 12:  - ambient
					// 13 - 15: - diffuse
					// 16 - 18: - specular
					// 19 		- cutOff
					// 20 		- outerCutOff
					gameplayManager->AddComponent(
						entity,
						Light(glm::vec3(it2.value()[1], it2.value()[2], it2.value()[3]),
							  glm::vec3(it2.value()[4], it2.value()[5], it2.value()[6]),
							  it2.value()[7],
							  it2.value()[8],
							  it2.value()[9],
							  glm::vec3(it2.value()[10], it2.value()[11], it2.value()[12]),
							  glm::vec3(it2.value()[13], it2.value()[14], it2.value()[15]),
							  glm::vec3(it2.value()[16], it2.value()[17], it2.value()[18]),
							  it2.value()[19],
							  it2.value()[20])
					);
				}		
				
			}
			else if (it2.key() == "Animator")
			{
				// 0 	- number of Animations
				// 1 	- name of the IdleAnimation
				// 2x	- path to animation file
				// 3x	- looping option (true/false)

				std::vector<std::pair<std::string, bool>> animations;
				
				int numOfAnimations = it2.value()[0];

				for (int i = 0; i < numOfAnimations * 2; i += 2)
				{
					std::pair<std::string, bool> animation (it2.value()[2+i], it2.value()[3+i]);
					animations.push_back(animation);
				}
				gameplayManager->AddComponent(
							entity,
							Animator(animations));
				
			}
			
		}
	}
}