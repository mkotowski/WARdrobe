#include "InputManager.hpp"

#include <iostream>

InputManager::InputManager() {}

InputManager::~InputManager() {}

void
InputManager::AddKey(int key, int mods)
{
	trackedKeys.insert({ std::make_pair(key, mods) });
	keyValues.insert({ std::make_pair(key, mods), std::make_pair(0, 0) });
}

void
InputManager::AddButton(int mouse_button, int mods)
{
	trackedButtons.insert({ std::make_pair(mouse_button, mods) });
	buttonValues.insert(
	  { std::make_pair(mouse_button, mods), std::make_pair(0, 0) });
}

void
InputManager::AddGamepad(int gamepadId)
{
	conectedGamepads.insert(gamepadId);
}

void
InputManager::AddGamepadAxis(int gamepadId, int gamepadAxis)
{}

void
InputManager::AddGamepadButton(int gamepadId, int gamepadButton)
{}

void
InputManager::SetCursorTracking(int isTracked)
{}

void
InputManager::SetScrollTracking(int isTracked)
{}

void
InputManager::UpdateKey(int key, int action, int mods)
{
	auto keyData = std::make_pair(key, mods);

	if (trackedKeys.find(keyData) != trackedKeys.end()) {

		// Move the old state to the first position
		std::swap(keyValues[keyData].first, keyValues[keyData].second);

		auto oldState = keyValues[keyData].first;
		keyValues[keyData].second = action;

		std::cout << "Key updated! " << keyValues[keyData].first
		          << keyValues[keyData].second << " " << mods;

		// Check the new state
		if (action == GLFW_RELEASE) {
			if (oldState == GLFW_RELEASE) {
				// The key is idle for pair <0,0>
				std::cout << "\t[IDLE]\n";
			}

			if (oldState == GLFW_PRESS || oldState == GLFW_REPEAT) {
				// The key is released for pairs <1,0> and <2,0>
				std::cout << "\t[RELEASED]\n";
			}
		}
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			if (oldState == GLFW_RELEASE) {
				// The key is pressed for pair <0,1>
				std::cout << "\t[PRESSED]\n";
			}

			if (oldState == GLFW_PRESS || oldState == GLFW_REPEAT) {
				// The key is on repeat for pairs <1,1> (unlikely) and <2,2> and <1,2>
				std::cout << "\t[REPEAT]\n";
			}
		}
	}
}

void
InputManager::UpdateButton(int button, int action, int mods)
{
	auto buttonData = std::make_pair(button, mods);

	if (trackedButtons.find(buttonData) != trackedButtons.end()) {

		// Move the old state to the first position
		std::swap(buttonValues[buttonData].first, buttonValues[buttonData].second);

		auto oldState = buttonValues[buttonData].first;
		buttonValues[buttonData].second = action;

		std::cout << "Button updated! " << buttonValues[buttonData].first
		          << buttonValues[buttonData].second << " " << mods;

		// Check the new state
		if (action == GLFW_RELEASE) {
			if (!isMouseButtonHoldDown[button]) {
				// The key is idle for pair <0,0>
				std::cout << "\t[IDLE]\n";
			}

			if (isMouseButtonHoldDown[button]) {
				// The key is released for pairs <1,0> and <2,0>
				std::cout << "\t[RELEASED]\n";
			}
		}
		if (action == GLFW_PRESS) {
			if (!isMouseButtonHoldDown[button]) {
				// The key is pressed for pair <0,1>
				std::cout << "\t[PRESSED]\n";
			}

			if (isMouseButtonHoldDown[button]) {
				// The key is on repeat for pairs <1,1> (unlikely) and <2,2> and <1,2>
				std::cout << "\t[REPEAT]\n";
			}
		}

		if (action == GLFW_PRESS) {
			isMouseButtonHoldDown[button] = true;
		} else {
			isMouseButtonHoldDown[button] = false;
		}
	}

	/*std::cout << "Buttons: [ ";

	for (auto& button : trackedButtons) {
	  std::cout << isMouseButtonHoldDown[std::get<0>(button)] << " ";
	}

	std::cout << "]" << std::endl;*/
}

void
InputManager::UpdateGamepad(int jid)
{
	if (glfwJoystickIsGamepad(jid)) {
		GLFWgamepadstate state;

		if (glfwGetGamepadState(jid, &state)) {
			std::cout << "Buttons pressed: ";

			for (int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST; i++) {
				std::cout << static_cast<int>(state.buttons[i]);
			}

			for (int i = 0; i < GLFW_GAMEPAD_AXIS_LAST; i++) {
				std::cout << " " << static_cast<float>(state.axes[i]) << " ";
			}

			std::cout << std::endl;
		}
	} else {
		// remove gamepad!
	}
}

void
InputManager::RemoveKey(int key, int mods)
{
	auto keyData = std::make_pair(key, mods);
	trackedKeys.erase(keyData);
	keyValues.erase(keyData);
}

void
InputManager::RemoveButton(int mouse_button, int mods)
{
	auto buttonData = std::make_pair(mouse_button, mods);
	trackedButtons.erase(buttonData);
	buttonValues.erase(buttonData);
}

void
InputManager::RemoveGamepad(int gamepadId)
{
	conectedGamepads.erase(gamepadId);
}

void
InputManager::RemoveGamepadAxis(int gamepadId, int gamepadAxis)
{}

void
InputManager::RemoveGamepadButton(int gamepadId, int gamepadButton)
{}

InputData
InputManager::Update(GLFWwindow* window)
{
	InputData data;

	/*for (auto& key : trackedKeys) {
	  int state = glfwGetKey(window, std::get<0>(key));
	  UpdateKey(std::get<0>(key), state, std::get<1>(key));
	  // UpdateKey(std::get<0>(key), state, std::get<1>(key));
	}*/

	/*for (auto& button : trackedButtons) {
	  isMouseButtonHoldDown[std::get<0>(button)] =
	    static_cast<bool>(glfwGetMouseButton(window, std::get<0>(button)));

	  buttonValues[button].first = buttonValues[button].second;
	  buttonValues[button].second =
	    glfwGetMouseButton(window, std::get<0>(button));
	}*/

	// all gamepads data will be conflated to single input
	// na zasadzie input A z pada 1 OR input A z pada 2 OR ...
	for (auto& gamepad : conectedGamepads) {
		UpdateGamepad(gamepad);
	}

	return data;
}