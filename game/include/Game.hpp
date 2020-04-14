#pragma once

#include "Version.h"
#include "Constants.h"

#include "Window.hpp"
#include "ecs.hpp"

class Game
{
public:
	Game(std::string windowTitle);
	~Game();

	Window* GetGameWindow() { return gameWindow; }

	GameplayManager* gameplayManager;

	void Loop();

private:
	Window* gameWindow;
};