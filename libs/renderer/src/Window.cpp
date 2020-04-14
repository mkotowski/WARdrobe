#include <iostream>
#include <stdio.h>
#include <clocale>

#include "Window.hpp"


static void
glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
	}

	std::cout << "The key " << scancode << " : " << scancode << " mods: " << mods
	          << "\n";
}

void
drop_callback(GLFWwindow* window, int count, const char** paths)
{
	std::setlocale(LC_ALL, "pl_PL.UTF-8");
	for (int i = 0; i < count; i++)
		// handle_dropped_file(paths[i]);
		std::cout << "File dropped: " << paths[i];
}

void
Window::ProcessInput()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	GLFWgamepadstate state;

	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
		if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS) {
			std::cout << "Gamepad A\n";
		}

		if (abs(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]) >= 0.01f) {
			std::cout << "Axis Right X: " << state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] << "\n";
		}

		if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS) {
			std::cout << "Gamepad B\n";
		}
	}
}

void
window_close_callback(GLFWwindow* window)
{
	// if (!time_to_close)
	glfwSetWindowShouldClose(window, GL_TRUE);
}

void
framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width
	// and height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	// Re-render the scene because the current frame was drawn for the old
	// resolution
	// draw();
}

void
monitor_callback(GLFWmonitor* monitor, int event)
{
	if (event == GLFW_CONNECTED) {
		// The monitor was connected
	} else if (event == GLFW_DISCONNECTED) {
		// The monitor was disconnected
	}
}

void
joystick_callback(int jid, int event)
{
	if (event == GLFW_CONNECTED) {
		// The joystick was connected
		std::cout << "Joystick connected: " << glfwGetJoystickGUID(jid) << "\n";
		if (glfwGetGamepadName(jid)) {
			std::cout << "Gamepad name: " << glfwGetGamepadName(jid) << "\n";
		}
	} else if (event == GLFW_DISCONNECTED) {
		// The joystick was disconnected
		std::cout << "Joystick disconnected\n";
	}
}

void
mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
	}

	// Advanced Buttons
	// XButton1	4th mouse button. Typically performs the same function as
	// Browser_Back. XButton2	5th mouse button. Typically performs the same
	// function as Browser_Forward.

	// GLFW_MOUSE_BACK
	if (button == GLFW_MOUSE_BUTTON_4 && action == GLFW_PRESS) {
		// SetShouldClose(GLFW_TRUE);
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	// GLFW_MOUSE_FORWARD
	if (button == GLFW_MOUSE_BUTTON_5 && action == GLFW_PRESS) {
		// SetShouldClose(GLFW_TRUE);
		glClearColor(0, 0, 0, 1);
	}
}

template<typename Function>
void
Window::SetDropCallback(Function drop_callback)
{
	glfwSetDropCallback(window, drop_callback);
}

template<typename Function>
void
Window::SetMouseButtonCallback(Function mouse_button_callback)
{
	glfwSetMouseButtonCallback(window, mouse_button_callback);
}

template<typename Function>
void
Window::SetWindowCloseCallback(Function window_close_callback)
{
	glfwSetWindowCloseCallback(window, window_close_callback);
}

template<typename Function>
void
Window::SetMonitorCallback(Function monitor_callback)
{
	// If a monitor is disconnected, all windows that are full screen
	// on it will be switched to windowed mode before the callback is called
	glfwSetMonitorCallback(monitor_callback);
}

template<typename Function>
void
Window::SetErrorCallback(Function glfw_error_callback)
{
	glfwSetErrorCallback(glfw_error_callback);
}

Window::Window(std::string windowTitle)
  : windowTitle(windowTitle)
{
	int err = Setup();

	if (err == 1) {
		std::cout << "Error while initializing GLFW!\n";
	} else {
		SetGLAndGLSLVersions();

		if (CreateContext() == 1) {
			std::cout << "Error while creating graphical context GLFW!\n";
		}

		if (InitializeOpenGLLoader() == 1) {
			std::cout << "Error while initializing OpenGL loader!\n";
		}

		// Set GLFW window callbacks
		SetWindowCloseCallback(window_close_callback);
		SetDropCallback(drop_callback);
		SetMouseButtonCallback(mouse_button_callback);
		SetMonitorCallback(monitor_callback);
		glfwSetKeyCallback(window, key_callback);
		glfwSetJoystickCallback(joystick_callback);

		/*const char* pctwinshock =
		  "03000000790000000600000000000000,G-Shark "
		  "GS-GP702,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0."
		  "1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,"
		  "rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a4,"
		  "start:b9,x:b3,y:b0,platform:Windows,";*/

		const char* pctwinshock =
		  "03000000790000000600000000000000,G-Shark "
		  "GS-GP702,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0."
		  "1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,"
		  "rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,"
		  "start:b9,x:b3,y:b0,platform:Windows,";

		if(glfwUpdateGamepadMappings(pctwinshock)) {
			std::cout << "Game mapping updated!\n";
		}

#if INCLUDE_DEBUG_UI
		debugUi = new DebugUI(window, glslVersion.c_str());
#endif // INCLUDE_DEBUG_UI

		SetClearColor(0.45f, 0.55f, 0.60f, 0.00f);
		// SetClearColor(0.00f, 0.00f, 0.00f, 0.00f);

		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
		SetViewport(0, 0, framebufferWidth, framebufferHeight);

		char                 version = (char)glGetString(GL_VERSION);
		const unsigned char* glver = glGetString(GL_VERSION);
		std::cout << "Detected OpenGL version: " << glver << std::endl;
	}
}

Window::~Window()
{
	Close();
}

int
Window::Setup()
{
	// Setup window
	SetErrorCallback(glfw_error_callback);

	if (!glfwInit()) {
		return 1;
	}

	return 0;
}

void
Window::SetGLAndGLSLVersions()
{
	// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glslVersion = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
	// GL 4.3 + GLSL 430
	const char* glsl_version = "#version 330 core";
	glslVersion = glsl_version;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only
#endif
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	// glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
}

int
Window::CreateContext()
{
	// Create window with graphics context
	window = glfwCreateWindow(1280, 720, windowTitle.c_str(), NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	return 0;
}

int
Window::InitializeOpenGLLoader()
{
	// Initialize OpenGL loader
	bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (err) {
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	return 0;
}

int
Window::ShouldClose()
{
	return glfwWindowShouldClose(window);
}

void
Window::PollEvents()
{
	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
	// tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to
	// your main application.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
	// data to your main application. Generally you may always pass all inputs
	// to dear imgui, and hide them from your application based on those two
	// flags.
	glfwPollEvents();
}

#if INCLUDE_DEBUG_UI
void
Window::RenderDebugUI()
{
	debugUi->GetNewFrame();
	debugUi->SetupWidgets();
	debugUi->RenderFrame();
	debugUi->DrawFrame();
}
#endif // INCLUDE_DEBUG_UI

void
Window::SetShouldClose(int glfwBool)
{
	glfwSetWindowShouldClose(window, glfwBool);
}

void
Window::SetTitle(std::string windowTitle)
{
	glfwSetWindowTitle(window, windowTitle.c_str());
}

void
Window::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	glViewport(x, y, width, height);
}

void
Window::SetClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	glClearColor(red, green, blue, alpha);
}

void
Window::ClearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void
Window::SwapBuffers()
{
	// GLFW windows by default use double buffering. That means that each
	// window has two rendering buffers; a front buffer and a back buffer.
	// The front buffer is the one being displayed and the back buffer the one
	// you render to.
	//
	// When the entire frame has been rendered, the buffers need to be swapped
	// with one another, so the back buffer becomes the front buffer and vice
	// versa.
	glfwSwapBuffers(window);
}

void
Window::UpdateViewport()
{}

void
Window::Close()
{
	// Cleanup

#if INCLUDE_DEBUG_UI
	delete debugUi;
	debugUi = nullptr;
#endif // INCLUDE_DEBUG_UI

	glfwDestroyWindow(window);
	glfwTerminate();
}