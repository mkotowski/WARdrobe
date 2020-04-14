#pragma once

#include <memory>
#include <unordered_map>

class Window;

class InputHandler
{
public:
	InputHandler(Window* window);
	~InputHandler();

	void SetWindow(Window* window);

private:
	// input source
	Window* window = nullptr;
};
