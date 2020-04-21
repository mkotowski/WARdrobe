#pragma once

#include <string>

// About OpenGL function loaders: modern OpenGL doesn't have a standard header
// file and requires individual function pointers to be loaded manually. Helper
// libraries are often used for this purpose! Here we are supporting a few
// common ones: gl3w, glew, glad. You may use another loader/header of your
// choice (glext, glLoadGen, etc.), or chose to manually implement your own.

#include <glad/glad.h> // Initialize with gladLoadGL()
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions

#include "Constants.h"

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

#if INCLUDE_DEBUG_UI
	DebugUI*    GetDebugUI() { return debugUi; }
#endif // INCLUDE_DEBUG_UI

	void ProcessInput();

private:
	GLFWwindow* window;

#if INCLUDE_DEBUG_UI
	DebugUI*    debugUi;
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
};