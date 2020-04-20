#pragma once

#include <vector>

#include "Constants.h"
#include "Version.h"

#include "Window.hpp"
#include "ecs.hpp"

class Game
{
public:
	Game(std::string windowTitle);
	~Game();

	Window* GetGameWindow() { return gameWindow; }

	GameplayManager* gameplayManager;
	Logger*          logger;

	void Loop();

private:
	Window* gameWindow;
};