#include "InputHandler.hpp"

InputHandler::InputHandler(Window* window)
{
	SetWindow(window);
}

InputHandler::~InputHandler() {}

void
InputHandler::SetWindow(Window* window)
{
	this->window = window;
}