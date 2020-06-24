#include "InputManager.hpp"

#include <iostream>

#include "Constants.h"

InputManager::InputManager() {}

InputManager::~InputManager() {}

void
InputManager::AddKey(int key, int state, int mods)
{
	// trackedKeys.insert({ std::make_pair(key, mods) });
	// keyValues.insert({ std::make_pair(key, mods), std::make_pair(0, 0) });

	AddInput(key, InputSource::KEY, mods);
}

void
InputManager::AddButton(int mouse_button, int state, int mods)
{
	// trackedButtons.insert({ std::make_pair(mouse_button, mods) });
	// buttonValues.insert(
	//  { std::make_pair(mouse_button, mods), std::make_pair(0, 0) });

	AddInput(mouse_button, InputSource::MOUSE_BUTTON, mods);
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
	/*auto keyData = std::make_pair(key, mods);

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
	}*/

	InputManager::UpdateInput(key, InputSource::KEY, action, mods);
}

void
InputManager::UpdateButton(int button, int action, int mods)
{
	/*auto buttonData = std::make_pair(button, mods);

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

	}*/

	UpdateInput(button, InputSource::MOUSE_BUTTON, action, mods);

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
InputManager::RemoveKey(int key, int state, int mods)
{
	// auto keyData = std::make_pair(key, mods);
	// trackedKeys.erase(keyData);
	// keyValues.erase(keyData);

	inputs.remove(Input(key, InputSource::KEY, state, mods));
}

void
InputManager::RemoveButton(int mouse_button, int state, int mods)
{
	// auto buttonData = std::make_pair(mouse_button, mods);
	// trackedButtons.erase(buttonData);
	// buttonValues.erase(buttonData);

	inputs.remove(Input(mouse_button, InputSource::MOUSE_BUTTON, state, mods));
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

void
InputManager::Update(GLFWwindow* window)
{
	/*for (auto& key : trackedKeys) {
	  int state = glfwGetKey(window, std::get<0>(key));
	  UpdateKey(std::get<0>(key), state, std::get<1>(key));
	  // UpdateKey(std::get<0>(key), state, std::get<1>(key));
	}*/

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

	Call();
}

void
InputManager::AddInput(int id, int source, int mods)
{

	Input input;
	input.id = id;
	input.source = source;
	input.state = InputState::IDLE;
	input.mods = mods;

	inputs.push_back(input);

	std::cout << "Input " << input.id << " from " << input.source << " added!\n";
}

void
InputManager::BindAction(int             id,
                         int             source,
                         int             state,
                         int             mods,
                         CallbackPointer callbackPtr)
{
	Input input;
	input.id = id;
	input.source = source;
	input.state = state;
	input.mods = mods;

	//(*callbackPtr)();
	// std::cout << typeid(callbackPtr).name() << std::endl;

	AddInput(input.id, input.source, input.mods);
	bindings.insert(std::make_pair(input, callbackPtr));
}

void
InputManager::UpdateInput(int id, int source, int state, int mods)
{
	if (id == GLFW_KEY_W && source == InputSource::KEY) {
		forwardInput += 0.1f;
		playerInputVertical += 0.1f;
	}

	if (id == GLFW_KEY_S && source == InputSource::KEY) {
		forwardInput -= 0.1f;
		playerInputVertical -= 0.1f;
	}

	if (id == GLFW_KEY_D && source == InputSource::KEY) {
		rightInput += 0.1f;
		playerInputHorizontal += 0.1f;
	}

	if (id == GLFW_KEY_A && source == InputSource::KEY) {
		rightInput -= 0.1f;
		playerInputHorizontal -= 0.1f;
	}

	if (id == GLFW_KEY_SPACE && source == InputSource::KEY &&
	    state == GLFW_PRESS) {
		leftMousePressed = true;
	} else if (id == GLFW_KEY_SPACE && source == InputSource::KEY &&
	           state == GLFW_RELEASE) {
		leftMousePressed = false;
	}

	if (id == GLFW_KEY_B && source == InputSource::KEY &&
	    state == GLFW_PRESS) {
		rightMousePressed = true;
	} else if (id == GLFW_KEY_B && source == InputSource::KEY &&
	           state == GLFW_RELEASE) {
		rightMousePressed = false;
	}

	if (id == GLFW_KEY_E && source == InputSource::KEY && state == GLFW_PRESS) {
		ePressed = true;
	} else if (id == GLFW_KEY_E && source == InputSource::KEY &&
	           state == GLFW_RELEASE) {
		ePressed = false;
	}

	if (id == GLFW_KEY_Q && source == InputSource::KEY && state == GLFW_PRESS) {
		qPressed = true;
	} else if (id == GLFW_KEY_Q && source == InputSource::KEY &&
	           state == GLFW_RELEASE) {
		qPressed = false;
	}

	Input searchedInput;
	searchedInput.id = id;
	searchedInput.source = source;
	searchedInput.state = state;
	searchedInput.mods = mods;

	// std::list<Input>::iterator
	auto it = std::find_if(
	  inputs.begin(), inputs.end(), [id, source, state, mods](const Input& i) {
		  return (id == i.id) && (source == i.source) && (mods == i.mods);
		  // Input::input_type_compare(i, searchedInput)
	  });

	if (it != inputs.end()) {
		it->state = state;
		std::cout << "Input " << it->id << " from " << it->source
		          << " updated to state " << it->state << "!\n";
	}
}

void
InputManager::Call()
{
	for (auto input : inputs) {
		auto it = bindings.find(input);

		if (it != bindings.end()) {
			// std::cout << "Activate callback binded to input " << (&input)->id
			//          << " from source " << (&input)->source << "!\n";
			(*(it->second))(input.state);
		}
	}
}