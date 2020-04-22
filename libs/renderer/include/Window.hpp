#pragma once

#include <string>

// About OpenGL function loaders: modern OpenGL doesn't have a standard header
// file and requires individual function pointers to be loaded manually. Helper
// libraries are often used for this purpose! Here we are supporting a few
// common ones: gl3w, glew, glad. You may use another loader/header of your
// choice (glext, glLoadGen, etc.), or chose to manually implement your own.

// Initialize with gladLoadGL()
#include <glad/glad.h>
// Include glfw3.h **after** our OpenGL definitions
#include <GLFW/glfw3.h>

#include "Constants.h"
#include "InputManager.hpp"

#if INCLUDE_DEBUG_UI // INCLUDE_DEBUG_UI
#include "DebugUI.hpp"
#else
#include "ConfigManager.hpp"
#endif

class Window
{
public:
	Window(std::string windowName);
	~Window();

	void SetTitle(std::string windowTitle);

	void SetShouldClose(int glfwBool);
	void PollEvents();
	void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height);
	void SetClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void ClearScreen();
	void RenderDebugUI();
	void SwapBuffers();
	void UpdateViewport();

	int ShouldClose();

	int GetWindowWidth();
	int GetWindowHeight();

	GLFWwindow* GetWindow() { return window; }

	// GLFW Callback Setters

	template<typename Function>
	void SetDropCallback(Function drop_callback);

	template<typename Function>
	void SetMouseButtonCallback(Function mouse_button_callback);

	template<typename Function>
	void SetWindowCloseCallback(Function window_close_callback);

	template<typename Function>
	void SetMonitorCallback(Function monitor_callback);

	template<typename Function>
	void SetErrorCallback(Function glfw_error_callback);

	template<typename Function>
	void SetKeyCallback(Function key_callback);

	template<typename Function>
	void SetScrollCallback(Function scroll_callback);

	template<typename Function>
	void SetJoystickCallback(Function joystick_callback);

	// Default GLFW Callbacks
	// For accessing the Window instance memebers:
	// @look https://stackoverflow.com/questions/7676971/
	// pointing-to-a-function-that-is-a-class-member-glfw-setkeycallback

	static void DefaultKeyCallback(GLFWwindow* window,
	                               int         key,
	                               int         scancode,
	                               int         action,
	                               int         mods);

	static void DefaultWindowCloseCallback(GLFWwindow* window);

	static void DefaultDropCallback(GLFWwindow*  window,
	                                int          count,
	                                const char** paths);

	static void DefaultMouseButtonCallback(GLFWwindow* window,
	                                       int         button,
	                                       int         action,
	                                       int         mods);

	static void DefaultFramebufferSizeCallback(GLFWwindow* window,
	                                           int         width,
	                                           int         height);

	static void DefaultMonitorCallback(GLFWmonitor* monitor, int event);

	static void DefaultScrollCallback(GLFWwindow* window,
	                                  double      xoffset,
	                                  double      yoffset);

	static void DefaultJoystickCallback(int jid, int event);

#if INCLUDE_DEBUG_UI
	DebugUI* GetDebugUI() { return debugUi; }
#endif // INCLUDE_DEBUG_UI

	void ProcessInput();

	std::shared_ptr<InputManager> GetInputManager() { return input; }

	static Window* mainWindowPtr;

private:
	GLFWwindow* window;

#if INCLUDE_DEBUG_UI
	DebugUI* debugUi;
#endif // INCLUDE_DEBUG_UI

	std::string windowTitle;
	std::string glslVersion;

	int framebufferWidth;
	int framebufferHeight;

	void SetGLAndGLSLVersions();
	void Close();

	int Setup();
	int CreateContext();
	int InitializeOpenGLLoader();

	std::shared_ptr<InputManager> input = nullptr;
};