#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ConfigManager.hpp"

class DebugUI
{
public:
	DebugUI(GLFWwindow* window, const char* glsl_version);
	~DebugUI();

	void CreateContext();
	void GetNewFrame();
	void SetupWidgets();
	void RenderFrame();
	void DrawFrame();

private:
	// GLFW Window hints (properties)
	bool borderlessWindow;
	bool transparentWindow = false;
	bool fullScreenWindow;
	bool resizableWindow;

	bool wireframeMode = false;
	bool vsyncEnabled;

	// the number of monitors detected
	int monitorCount;
	// the index of the fullscreen monitor target, by default the primary monitor
	int fullScreenMonitor = 0;

	// an array of handles for all currently connected monitors
	GLFWmonitor** monitors;

	void fullScreenWindowToggle();

	GLFWwindow* parentWindow;
};
