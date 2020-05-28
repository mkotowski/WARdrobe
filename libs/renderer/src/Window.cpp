#include <clocale>
#include <iostream>
#include <stdio.h>

#include "Window.hpp"

static void
glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void
Window::DefaultDropCallback(GLFWwindow* window, int count, const char** paths)
{
	for (int i = 0; i < count; i++) {
		// handle dropped file
		std::cout << "File dropped: " << paths[i];
	}
}

void
Window::ProcessInput()
{
	input->Update(this->window);

	/*if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
	  glfwSetWindowShouldClose(window, true);
	}

	GLFWgamepadstate state;

	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
	  if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS) {
	    std::cout << "Gamepad A\n";
	  }

	  if (abs(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]) >= 0.01f) {
	    std::cout << "Axis Right X: " << state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]
	              << "\n";
	  }

	  if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS) {
	    std::cout << "Gamepad B\n";
	  }
	}*/
}

void
Window::DefaultWindowCloseCallback(GLFWwindow* window)
{
	// if (!time_to_close)
	glfwSetWindowShouldClose(window, GL_TRUE);
}

void
Window::DefaultFramebufferSizeCallback(GLFWwindow* window,
                                       int         width,
                                       int         height)
{
	// make sure the viewport matches the new window dimensions; note that width
	// and height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	// Re-render the scene because the current frame was drawn for the old
	// resolution
	// draw();
}

void
Window::DefaultScrollCallback(GLFWwindow* window,
                              double      xoffset,
                              double      yoffset)
{
	std::cout << "Scroll: " << xoffset << " " << yoffset << std::endl;
}

void
Window::DefaultMonitorCallback(GLFWmonitor* monitor, int event)
{
	if (event == GLFW_CONNECTED) {
		// The monitor was connected
	} else if (event == GLFW_DISCONNECTED) {
		// The monitor was disconnected
	}
}

void
Window::DefaultJoystickCallback(int jid, int event)
{
	Window* handler = Window::mainWindowPtr;

	glfwSetJoystickUserPointer(jid, mainWindowPtr);

	std::cout << jid << " " << glfwJoystickIsGamepad(jid) << " "
	          << glfwGetJoystickUserPointer(jid) << std::endl;

	if (handler != NULL) {
		if (event == GLFW_CONNECTED) {
			// The joystick was connected
			handler->input->AddGamepad(jid);

			std::cout << "Joystick connected (" << glfwGetJoystickGUID(jid) << ") ";
			if (glfwGetGamepadName(jid)) {
				std::cout << glfwGetGamepadName(jid) << "\n";
			}
		} else if (event == GLFW_DISCONNECTED) {
			// The joystick was disconnected
			handler->input->RemoveGamepad(jid);

			std::cout << "Joystick disconnected!\n";
		}
	}
}

void
Window::DefaultMouseButtonCallback(GLFWwindow* window,
                                   int         button,
                                   int         action,
                                   int         mods)
{
	Window* handler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

	handler->input->UpdateButton(button, action, mods);

	// Advanced Buttons
	// XButton1	4th mouse button. Typically performs the same function as
	// Browser_Back. XButton2	5th mouse button. Typically performs the same
	// function as Browser_Forward.

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		// SetShouldClose(GLFW_TRUE);
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	// GLFW_MOUSE_BACK
	if (button == GLFW_MOUSE_BUTTON_4 && action == GLFW_PRESS) {
		// SetShouldClose(GLFW_TRUE);
		glfwSetWindowShouldClose(window, GLFW_TRUE);
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
Window::SetKeyCallback(Function key_callback)
{
	glfwSetKeyCallback(window, key_callback);
}

template<typename Function>
void
Window::SetErrorCallback(Function glfw_error_callback)
{
	glfwSetErrorCallback(glfw_error_callback);
}

template<typename Function>
void
Window::SetScrollCallback(Function scroll_callback)
{
	glfwSetScrollCallback(window, scroll_callback);
}

template<typename Function>
void
Window::SetJoystickCallback(Function joystick_callback)
{
	glfwSetJoystickCallback(joystick_callback);
}

void
Window::DefaultKeyCallback(GLFWwindow* window,
                           int         key,
                           int         scancode,
                           int         action,
                           int         mods)
{
	Window* handler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

	handler->input->UpdateKey(key, action, mods);
}

Window* Window::mainWindowPtr = nullptr;

class Tmp
{
public:
	double  beforeState = -1;
	double    currentState = 0;
	void execute(double data)
	{
		std::cout << "Call from TMP"
		          << "\n";
		beforeState = currentState;
		currentState = data;
	}
};

Window::Window(std::string windowTitle)
  : windowTitle(windowTitle)
{
	input = std::make_shared<InputManager>();

	/*input->AddKey(GLFW_KEY_0, GLFW_PRESS, 0);
	input->AddKey(GLFW_KEY_0, GLFW_PRESS, GLFW_MOD_ALT);
	input->AddKey(GLFW_KEY_ESCAPE, GLFW_PRESS, 0);

	input->AddButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
	input->AddButton(GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS, 0);
	input->AddButton(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS, 0);
	input->AddButton(GLFW_MOUSE_BUTTON_4, GLFW_PRESS, 0);
	input->AddButton(GLFW_MOUSE_BUTTON_5, GLFW_PRESS, 0);*/

	/*Tmp             a;
	Callback        callback = std::bind(&Tmp::execute, a, std::placeholders::_1);
	CallbackPointer ptr = std::make_shared<Callback>(callback);*/

	/*input->BindAction(
	  GLFW_MOUSE_BUTTON_LEFT, InputSource::MOUSE_BUTTON, GLFW_PRESS, 0, ptr);
	input->BindAction(
	  GLFW_MOUSE_BUTTON_RIGHT, InputSource::MOUSE_BUTTON, GLFW_PRESS, 0, ptr);
	input->BindAction(
	  GLFW_MOUSE_BUTTON_MIDDLE, InputSource::MOUSE_BUTTON, GLFW_PRESS, 0, ptr);
	input->BindAction(
	  GLFW_MOUSE_BUTTON_4, InputSource::MOUSE_BUTTON, GLFW_PRESS, 0, ptr);*/

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

		// mainWindowPtr = reinterpret_cast<void*>(this);
		mainWindowPtr = this;

		// https://discourse.glfw.org/t/what-is-a-possible-use-of-glfwgetwindowuserpointer/1294
		glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));

		// Set GLFW window callbacks
		SetWindowCloseCallback(DefaultWindowCloseCallback);
		SetDropCallback(DefaultDropCallback);
		SetMouseButtonCallback(DefaultMouseButtonCallback);
		SetMonitorCallback(DefaultMonitorCallback);
		SetKeyCallback(DefaultKeyCallback);
		SetJoystickCallback(DefaultJoystickCallback);
		SetScrollCallback(DefaultScrollCallback);

		const char* pctwinshock =
		  "03000000790000000600000000000000,G-Shark "
		  "GS-GP702,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0."
		  "1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,"
		  "rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,"
		  "start:b9,x:b3,y:b0,platform:Windows,";

		if (glfwUpdateGamepadMappings(pctwinshock)) {
			std::cout << "Game mapping updated!\n";
		}

		for (int jid = 0; jid < GLFW_JOYSTICK_LAST; jid++) {
			if (glfwJoystickPresent(jid)) {
				DefaultJoystickCallback(jid, GLFW_CONNECTED);
			}
		}

#if INCLUDE_DEBUG_UI
		debugUi = new DebugUI(window, glslVersion.c_str());
#endif // INCLUDE_DEBUG_UI

		SetClearColor(0.45f, 0.55f, 0.60f, 0.00f);

		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
		SetViewport(0, 0, framebufferWidth, framebufferHeight);
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
	const char* glsl_version = "#version 430 core";
	glslVersion = glsl_version;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only
	glfwWindowHint(GLFW_SAMPLES, 4); // <--- to be changed from setting files
#endif
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	// glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
}

int
Window::CreateContext()
{
	// Create window with graphics context

	Settings windowSettings = ConfigManager::GetSettings();

	window = glfwCreateWindow(windowSettings.size[0],
	                          windowSettings.size[1],
	                          windowTitle.c_str(),
	                          NULL,
	                          NULL);

	glfwSetWindowAttrib(window, GLFW_DECORATED, !windowSettings.borderless);

	std::string cursorMode = windowSettings.cursorMode;
	int         cursorModeValue = GLFW_CURSOR_NORMAL;

	if (cursorMode == "hidden") {
		cursorModeValue = GLFW_CURSOR_HIDDEN;
	} else if (cursorMode == "disabled") {
		cursorModeValue = GLFW_CURSOR_DISABLED;
	}

	glfwSetInputMode(window, GLFW_CURSOR, cursorModeValue);

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

int
Window::GetWindowWidth()
{
	return framebufferWidth;
}

int
Window::GetWindowHeight()
{
	return framebufferHeight;
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
{
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	SetViewport(0, 0, framebufferWidth, framebufferHeight);
}

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