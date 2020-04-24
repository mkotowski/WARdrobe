#pragma once

#include "Version.h"
#include "Constants.h"

#include "Window.hpp"
#include "ecs.hpp"

#include <nlohmann/json.hpp>

class Game
{
public:
	Game(std::string windowTitle);
	~Game();

	Window* GetGameWindow() { return gameWindow; }

	GameplayManager* gameplayManager;

	void Loop();
	void LoadLevel(std::string levelPath);

private:
	Window* gameWindow;
};