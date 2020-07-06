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
#include "ScriptsCommunicationFunctions.hpp"
#include "ScriptsSystem.hpp"
#include "ShaderSystem.hpp"

#include "AnimationSystem.hpp"
#include "LightningSystem.hpp"
#include "ScriptsSystem.hpp"

#include "ecs.hpp"

#include "ActionManager.hpp"

#include "InputManager.hpp"
#include "VirtualInputManager.hpp"

Game::Game(std::string windowTitle)
{
	gameWindow = new Window(windowTitle);

	//gameWindow->GetDebugUI()->AddLog("test imgui logger\n");

	gameplayManager = new GameplayManager();
}

Game::~Game() {}

void
Game::Loop()
{
	VirtualInputManager* vim = VirtualInputManager::GetInstance();

	vim->AddVirtualInput("MoveForward", VirtualInputType::Range);
	vim->AddVirtualInput("MoveRight", VirtualInputType::State);
	vim->AddVirtualInput("MoveBackward", VirtualInputType::State);
	vim->AddVirtualInput("MoveLeft", VirtualInputType::State);
	vim->AddVirtualInput("AttackLeftHand", VirtualInputType::Action);
	vim->AddVirtualInput("AttackRightHand", VirtualInputType::Action);
	vim->AddVirtualInput("MenuBack", VirtualInputType::Action);
	vim->AddVirtualInput("Restart", VirtualInputType::Action);
	vim->AddVirtualInput("ActivateHeal", VirtualInputType::Trigger);
	vim->AddVirtualInput("ActivateBuff", VirtualInputType::Trigger);

	vim->AddVirtualInput("RotationX", VirtualInputType::Raw);
	vim->AddVirtualInput("RotationY", VirtualInputType::Raw);

	vim->AddMapping(
	  "RotationX", GLFW_GAMEPAD_AXIS_RIGHT_X, InputSource::GAMEPAD_AXIS);
	vim->AddMapping(
	  "RotationY", GLFW_GAMEPAD_AXIS_RIGHT_Y, InputSource::GAMEPAD_AXIS);

	vim->AddMapping("RotationX", 0, InputSource::CURSOR);
	vim->AddMapping("RotationY", 1, InputSource::CURSOR);

	vim->AddMapping("MoveForward", GLFW_KEY_W, InputSource::KEY);
	vim->AddMapping("MoveForward", GLFW_KEY_UP, InputSource::KEY);
	vim->AddMapping(
	  "MoveForward", GLFW_GAMEPAD_BUTTON_DPAD_UP, InputSource::GAMEPAD_BUTTON);

	vim->AddMapping("ActivateHeal", GLFW_KEY_Q, InputSource::KEY);
	vim->AddMapping(
	  "ActivateHeal", GLFW_MOUSE_BUTTON_4, InputSource::MOUSE_BUTTON);
	vim->AddMapping(
	  "ActivateHeal", GLFW_GAMEPAD_BUTTON_X, InputSource::GAMEPAD_BUTTON);
	vim->AddMapping(
	  "ActivateHeal", GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, InputSource::GAMEPAD_AXIS);

	vim->AddMapping("ActivateBuff", GLFW_KEY_E, InputSource::KEY);
	vim->AddMapping(
	  "ActivateBuff", GLFW_MOUSE_BUTTON_5, InputSource::MOUSE_BUTTON);
	vim->AddMapping(
	  "ActivateBuff", GLFW_GAMEPAD_BUTTON_Y, InputSource::GAMEPAD_BUTTON);
	vim->AddMapping(
	  "ActivateBuff", GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, InputSource::GAMEPAD_AXIS);

	vim->AddMapping("MoveBackward", GLFW_KEY_S, InputSource::KEY);
	vim->AddMapping("MoveBackward", GLFW_KEY_DOWN, InputSource::KEY);
	vim->AddMapping(
	  "MoveBackward", GLFW_GAMEPAD_BUTTON_DPAD_DOWN, InputSource::GAMEPAD_BUTTON);

	vim->AddMapping("MoveRight", GLFW_KEY_D, InputSource::KEY);
	vim->AddMapping("MoveRight", GLFW_KEY_RIGHT, InputSource::KEY);
	vim->AddMapping(
	  "MoveRight", GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, InputSource::GAMEPAD_BUTTON);

	vim->AddMapping("MoveLeft", GLFW_KEY_A, InputSource::KEY);
	vim->AddMapping("MoveLeft", GLFW_KEY_LEFT, InputSource::KEY);
	vim->AddMapping(
	  "MoveLeft", GLFW_GAMEPAD_BUTTON_DPAD_LEFT, InputSource::GAMEPAD_BUTTON);

	vim->AddMapping("MenuBack", GLFW_KEY_ESCAPE, InputSource::KEY);
	vim->AddMapping(
	  "MenuBack", GLFW_GAMEPAD_BUTTON_BACK, InputSource::GAMEPAD_BUTTON);

	vim->AddMapping("Restart", GLFW_KEY_F5, InputSource::KEY);
	vim->AddMapping(
	  "Restart", GLFW_GAMEPAD_BUTTON_START, InputSource::GAMEPAD_BUTTON);

	vim->AddMapping(
	  "AttackLeftHand", GLFW_MOUSE_BUTTON_LEFT, InputSource::MOUSE_BUTTON);
	vim->AddMapping("AttackLeftHand", GLFW_KEY_SPACE, InputSource::KEY);
	vim->AddMapping(
	  "AttackLeftHand", GLFW_GAMEPAD_BUTTON_A, InputSource::GAMEPAD_BUTTON);
	vim->AddMapping("AttackLeftHand",
	                GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
	                InputSource::GAMEPAD_BUTTON);

	vim->AddMapping(
	  "AttackRightHand", GLFW_MOUSE_BUTTON_RIGHT, InputSource::MOUSE_BUTTON);
	vim->AddMapping("AttackRightHand", GLFW_KEY_B, InputSource::KEY);
	vim->AddMapping(
	  "AttackRightHand", GLFW_GAMEPAD_BUTTON_B, InputSource::GAMEPAD_BUTTON);
	vim->AddMapping("AttackRightHand",
	                GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
	                InputSource::GAMEPAD_BUTTON);

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
	gameplayManager->RegisterComponent<Skybox>();

	// Register the systems used during the gameplay
	auto cameraSystem = gameplayManager->RegisterSystem<CameraSystem>();

	auto shaderSystem = gameplayManager->RegisterSystem<ShaderSystem>();

	auto physicsSystem = gameplayManager->RegisterSystem<PhysicsSystem>();

	auto lightningSystem = gameplayManager->RegisterSystem<LightningSystem>();

	auto animationSystem = gameplayManager->RegisterSystem<AnimationSystem>();

	auto colliderSystem = gameplayManager->RegisterSystem<ColliderSystem>();

	auto scriptsSystem = gameplayManager->RegisterSystem<ScriptsSystem>();

	auto renderSystem = gameplayManager->RegisterSystem<RenderSystem>();
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

	// ShaderSystem
	gameplayManager->SetRequiredComponent<ShaderSystem>(
	  gameplayManager->GetComponentType<Shader>());

	// RenderSystem
	gameplayManager->SetRequiredComponent<RenderSystem>(
	  gameplayManager->GetComponentType<Renderer>());

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

	// Initialize ShaderSystem and bound map of shaders to RenderSystem and
	// LightningSystem
	shaderSystem->Init(gameplayManager->GetComponentManager());
	std::cout << "ShaderSystem has been initialized" << std::endl;

	shaderSystem->cameraEntity = cameraSystem->cameraEntity;
	renderSystem->shaders = &shaderSystem->shaders;
	lightningSystem->shaders = &shaderSystem->shaders;

	std::cout << "Here" << std::endl;
	animationSystem->Init(gameplayManager->GetComponentManager());
	std::cout << "AnimationSystem has been initialized" << std::endl;

	colliderSystem->window = this->gameWindow;
	colliderSystem->camera =
	  &gameplayManager->GetComponentManager()->GetComponent<Camera>(
	    cameraSystem->cameraEntity);

	colliderSystem->ourShader =
	  &gameplayManager->GetComponentManager()->GetComponent<Shader>(
	    shaderSystem->shaders.at("boxShader"));

	colliderSystem->Initiate(gameplayManager->GetComponentManager());
	std::cout << "ColliderSystem has been initialized" << std::endl;

	renderSystem->Init(gameplayManager->GetComponentManager());
	std::cout << "RenderSystem has been initialized" << std::endl;

	auto DamagePillIndicator = gameWindow->guiManager->AddWidget(
	  "damage_pill_indicator",
	  std::make_shared<GuiWidget>("assets/images/UI/Pills/DamagePill.png",
	                              30.0f,
	                              30.0f,
	                              128,
	                              128,
	                              GuiAnchor::TR,
	                              gameWindow->GetWindow()));

	auto HealthPillIndicator = gameWindow->guiManager->AddWidget(
	  "health_pill_indicator",
	  std::make_shared<GuiWidget>("assets/images/UI/Pills/HealthPill.png",
	                              30.0f,
	                              158.0f,
	                              128,
	                              128,
	                              GuiAnchor::TR,
	                              gameWindow->GetWindow()));

	auto SpeedPillIndicator = gameWindow->guiManager->AddWidget(
	  "speed_pill_indicator",
	  std::make_shared<GuiWidget>("assets/images/UI/Pills/SpeedPill.png",
	                              30.0f,
	                              158.0f + 128.0f,
	                              128,
	                              128,
	                              GuiAnchor::TR,
	                              gameWindow->GetWindow()));

	auto HeadHit = gameWindow->guiManager->AddWidget(
	  "head_hit",
	  std::make_shared<GuiWidget>("assets/images/UI/Drugman/DrugmanHit.png",
	                              0.0f,
	                              0.0f,
	                              120,
	                              128,
	                              GuiAnchor::BC,
	                              gameWindow->GetWindow()));
	HeadHit->SetVisible(false);

	auto Head0 = gameWindow->guiManager->AddWidget(
	  "head_0",
	  std::make_shared<GuiWidget>("assets/images/UI/Drugman/Drugman0.png",
	                              0.0f,
	                              0.0f,
	                              120,
	                              128,
	                              GuiAnchor::BC,
	                              gameWindow->GetWindow()));
	Head0->SetVisible(false);

	auto Head20 = gameWindow->guiManager->AddWidget(
	  "head_20",
	  std::make_shared<GuiWidget>("assets/images/UI/Drugman/Drugman20.png",
	                              0.0f,
	                              0.0f,
	                              120,
	                              128,
	                              GuiAnchor::BC,
	                              gameWindow->GetWindow()));
	Head20->SetVisible(false);

	auto Head40 = gameWindow->guiManager->AddWidget(
	  "head_40",
	  std::make_shared<GuiWidget>("assets/images/UI/Drugman/Drugman40.png",
	                              0.0f,
	                              0.0f,
	                              120,
	                              128,
	                              GuiAnchor::BC,
	                              gameWindow->GetWindow()));
	Head40->SetVisible(false);

	auto Head60 = gameWindow->guiManager->AddWidget(
	  "head_60",
	  std::make_shared<GuiWidget>("assets/images/UI/Drugman/Drugman60.png",
	                              0.0f,
	                              0.0f,
	                              120,
	                              128,
	                              GuiAnchor::BC,
	                              gameWindow->GetWindow()));
	Head60->SetVisible(false);

	auto Head80 = gameWindow->guiManager->AddWidget(
	  "head_80",
	  std::make_shared<GuiWidget>("assets/images/UI/Drugman/Drugman80.png",
	                              0.0f,
	                              0.0f,
	                              120,
	                              128,
	                              GuiAnchor::BC,
	                              gameWindow->GetWindow()));
	Head80->SetVisible(false);

	auto Head100 = gameWindow->guiManager->AddWidget(
	  "head_100",
	  std::make_shared<GuiWidget>("assets/images/UI/Drugman/Drugman100.png",
	                              0.0f,
	                              0.0f,
	                              120,
	                              128,
	                              GuiAnchor::BC,
	                              gameWindow->GetWindow()));
	Head100->SetVisible(false);

	gameWindow->headManager = std::make_shared<HeadManager>(
	  Head0, Head20, Head40, Head60, Head80, Head100, HeadHit);

	gameWindow->headManager->SetHead(80, false);

	scriptsSystem->Init(
	  gameplayManager,
	  renderSystem,
	  this->gameWindow,
	  &gameplayManager->GetComponentManager()->GetComponent<Camera>(
	    cameraSystem->cameraEntity));

	gameWindow->CloseSplashScreen();
	// avoid displaying empty window
	gameWindow->ShowWindow();

	// double       previous = glfwGetTime();
	// double       lag = 0.0;
	/*std::chrono::milliseconds previous =
	  std::chrono::duration_cast<std::chrono::milliseconds>(
	    std::chrono::system_clock::now().time_since_epoch());*/

	const float SEC_PER_UPDATE = 0.016;

	auto startTime = std::chrono::high_resolution_clock::now();
	auto stopTime = std::chrono::high_resolution_clock::now();

	while (!gameWindow->ShouldClose()) {
		startTime = std::chrono::high_resolution_clock::now();

		// EVENT AND INPUT PROCESSING
		gameWindow->PollEvents();

		if (!gameWindow->IsFocused()) {
			continue;
		}

		gameWindow->ProcessInput();
		gameWindow->GetInputManager()->Call();

		// GAME LOGIC

		// gameplayManager->Update(dt);

		physicsSystem->Update(dt, gameplayManager->GetComponentManager());
		colliderSystem->Update(dt, gameplayManager->GetComponentManager());
		scriptsSystem->Update(dt, gameplayManager->GetComponentManager());

		// GAMEPLAY RENDERING
		gameWindow->UpdateViewport();
		gameWindow->ClearScreen();

		cameraSystem->Update(dt, gameplayManager->GetComponentManager());
		lightningSystem->Update(dt, gameplayManager->GetComponentManager());
		shaderSystem->Update(dt, gameplayManager->GetComponentManager());

		renderSystem->Draw(dt, gameplayManager->GetComponentManager());

		// USER INTERFACE RENDERING
		gameWindow->guiManager->Draw();
#if INCLUDE_DEBUG_UI
		gameWindow->RenderDebugUI();
#endif // INCLUDE_DEBUG_UI

		stopTime = std::chrono::high_resolution_clock::now();
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

	try {
		rawLevelData >> jsonLevelData;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	// int entityNr = 0;
	for (auto& it : jsonLevelData.items()) {
		Entity entity = gameplayManager->CreateEntity();

		/*std::cout << "Key " << entityNr << ": " << it.key() << std::endl;
		entityNr++;*/

		for (auto& it2 : it.value().items()) {
			if (it2.key() == "ModelArray") {
				gameplayManager->AddComponent(entity, ModelArray(it2.value()[0]));
				if (it2.value()[0] == 0) {
					// 0: - chechLevelOfDetail (false)
					// 1: - model Path
					// 2: - texture Path
					gameplayManager->GetComponent<ModelArray>(entity).zeroLevelModel =
					  Model(it2.value()[1], it2.value()[2]);

				} else {
					// 0: 		chechLevelOfDetail (true)
					// 1/3/5: - modelPath   (first/second/third LoD)
					// 2/4/6: - texturePath (first/second/third LoD)
					gameplayManager->GetComponent<ModelArray>(entity).firstLevelModel =
					  Model(it2.value()[1], it2.value()[2]);
					gameplayManager->GetComponent<ModelArray>(entity).secondLevelModel =
					  Model(it2.value()[3], it2.value()[4]);
					gameplayManager->GetComponent<ModelArray>(entity).thirdLevelModel =
					  Model(it2.value()[5], it2.value()[6]);
				}
			} else if (it2.key() == "Shader") {
				// 0: vertex Path
				// 1: fragment Path
				// 2. shader type
				std::string vertexShaderPath = it2.value()[0];
				std::string fragmentShaderPath = it2.value()[1];
				std::string shaderType = it2.value()[2];
				gameplayManager->AddComponent(entity,
				                              Shader(vertexShaderPath.c_str(),
				                                     fragmentShaderPath.c_str(),
				                                     shaderType));
			} else if (it2.key() == "Transform") {
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
			} else if (it2.key() == "BoundingBox") {
				// 0 - width
				// 1 - height
				// 2 - depth
				// 3 - 0-collider, 1-trigger
				// 4 - true/false - unmovable
				// 5 - tag (optional)
				std::string tag = it2.value()[5] != nullptr ? it2.value()[5] : "none";
				gameplayManager->AddComponent(entity,
				                              BoundingBox{ it2.value()[0],
				                                           it2.value()[1],
				                                           it2.value()[2],
				                                           it2.value()[3] == 1,
				                                           it2.value()[4],
				                                           tag });

			} else if (it2.key() == "Scripts") {
				std::list<std::string> scripts;

				for (auto script : it2.value()) {
					scripts.push_back(script);
				}

				gameplayManager->AddComponent(entity, Scripts{ scripts });
			} else if (it2.key() == "Light") {
				// 0 - type of the light
				// Directional Light
				if (it2.value()[0] == "directionalLight") {
					// 1  - 3:  - direction
					// 4  - 6:  - position
					// 7  - 9:  - ambient
					// 10 - 12: - diffuse
					// 13 - 15: - specular
					gameplayManager->AddComponent(
					  entity,
					  Light(
					    glm::vec3(it2.value()[1], it2.value()[2], it2.value()[3]),
					    glm::vec3(it2.value()[4], it2.value()[5], it2.value()[6]),
					    glm::vec3(it2.value()[7], it2.value()[8], it2.value()[9]),
					    glm::vec3(it2.value()[10], it2.value()[11], it2.value()[12]),
					    glm::vec3(it2.value()[13], it2.value()[14], it2.value()[15])));
				} else if (it2.value()[0] == "pointLight") {
					// 1  - 3:  - position
					// 4 		- constant
					// 5 		- linear
					// 6 		- quadratic
					// 7  - 9:  - ambient
					// 10 - 12: - diffuse
					// 13 - 15: - specular
					gameplayManager->AddComponent(
					  entity,
					  Light(
					    glm::vec3(it2.value()[1], it2.value()[2], it2.value()[3]),
					    it2.value()[4],
					    it2.value()[5],
					    it2.value()[6],
					    glm::vec3(it2.value()[7], it2.value()[8], it2.value()[9]),
					    glm::vec3(it2.value()[10], it2.value()[11], it2.value()[12]),
					    glm::vec3(it2.value()[13], it2.value()[14], it2.value()[15])));
				} else if (it2.value()[0] == "spotLight") {
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
					        it2.value()[20]));
				}

			} else if (it2.key() == "Animator") {
				// 0 	- number of Animations
				// 1 	- name of the IdleAnimation
				// 2x	- path to animation file
				// 3x	- looping option (true/false)
				// 4x	- animation Name

				std::vector<std::pair<std::string, bool>> animations;
				std::vector<std::string>                  animationNames;

				int         numOfAnimations = it2.value()[0];
				std::string idleAnimationName = it2.value()[1];

				for (int i = 0; i < numOfAnimations * 3; i += 3) {
					std::pair<std::string, bool> animation(it2.value()[2 + i],
					                                       it2.value()[3 + i]);
					animations.push_back(animation);
					animationNames.push_back(it2.value()[4 + i]);
				}
				gameplayManager->AddComponent(
				  entity, Animator(animations, animationNames, idleAnimationName));

			} else if (it2.key() == "Skybox") {
				// 0 - 5: - paths to cubemaps
				std::vector<std::string> faces{
					it2.value()[0], it2.value()[1], it2.value()[2],
					it2.value()[3], it2.value()[4], it2.value()[5],
				};
				gameplayManager->AddComponent(entity, Skybox(faces));
			}
		}
	}
}