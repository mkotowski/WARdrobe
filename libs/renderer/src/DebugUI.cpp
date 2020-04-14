#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "DebugUI.hpp"

DebugUI::DebugUI(GLFWwindow* window, const char* glsl_version)
{
	CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	parentWindow = window;

	this->monitors = glfwGetMonitors(&monitorCount);

	fullScreenMonitor = 0;

	// returns an array of handles for all currently connected monitors
	// the primary monitor is always first in the returned array
	// if no monitors were found, this function returns NULL.
	// this->monitors = glfwGetMonitors(&monitorCount);

	// if (monitorCount > 0 && monitors[0] != NULL) {
	//}
}

DebugUI::~DebugUI()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void
DebugUI::CreateContext()
{
	ImGui::CreateContext();
}

void
DebugUI::GetNewFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	// ImGui::SetCurrentContext();
	ImGui::NewFrame();
}

void
DebugUI::SetupWidgets()
{
	bool show_demo_window = true;
	bool show_another_window = true;

	glfwSwapInterval(static_cast<int>(vsyncEnabled));

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("GLFW Window")) {

			if (ImGui::MenuItem("Resizable", NULL, this->resizableWindow)) {
				resizableWindow = !resizableWindow;
				glfwSetWindowAttrib(parentWindow, GLFW_RESIZABLE, resizableWindow);
			}

			if (ImGui::MenuItem("Borderless", NULL, this->borderlessWindow)) {
				borderlessWindow = !borderlessWindow;
				glfwSetWindowAttrib(parentWindow, GLFW_DECORATED, !borderlessWindow);
			}

			if (ImGui::MenuItem("Full screen", NULL, this->fullScreenWindow)) {
				fullScreenWindow = !fullScreenWindow;

				fullScreenWindowToggle();
				// glfwSetInputMode(parentWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Rendering")) {
			if (ImGui::MenuItem("Wireframe mode", NULL, this->wireframeMode)) {
				wireframeMode = !wireframeMode;
				if (wireframeMode) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
			}

			if (ImGui::MenuItem("Fill mode", NULL, !(this->wireframeMode))) {
				wireframeMode = !wireframeMode;
				if (!wireframeMode) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Fullscreen Monitor")) {
			this->monitors = glfwGetMonitors(&monitorCount);

			// TODO: implement this with glfw monitor disconnect callback
			if (fullScreenMonitor >= monitorCount) {
				// reset the target mintor for fullscreen to primary
				fullScreenMonitor = 0;
			}

			if (monitorCount > 0) {
				for (int i = 0; i < monitorCount; i++) {
					const char* name = glfwGetMonitorName(monitors[i]);

					if (i == 0) {
						if (ImGui::MenuItem("Primary screen",
						                    NULL,
						                    (fullScreenMonitor == i) ? true : false)) {
							fullScreenMonitor = i;
						}
					} else {
						// precautionary check for the case of unplugging a monitor
						if (monitorCount > 1) {
							if (ImGui::MenuItem(
							      name, NULL, (fullScreenMonitor == i) ? true : false)) {
								fullScreenMonitor = i;

								// https://github.com/glfw/glfw/issues/1072
								glfwSwapInterval(static_cast<int>(vsyncEnabled));
							}
						}
					}
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Vsync")) {
			if (ImGui::MenuItem("VSync", NULL, this->vsyncEnabled)) {
				vsyncEnabled = !vsyncEnabled;

				glfwSwapInterval(static_cast<int>(vsyncEnabled));
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// 1. Show the big demo window (Most of the sample code is in
	// ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
	// ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair
	// to created a named window.
	{
		static float f = 0.0f;
		static int   counter = 0;

		ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!"
		                               // and append into it.

		ImGui::Text("This is some useful text."); // Display some text (you can
		                                          // use a format strings too)
		ImGui::Checkbox(
		  "Demo Window",
		  &show_demo_window); // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float",
		                   &f,
		                   0.0f,
		                   1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3(
		  "clear color",
		  (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button")) // Buttons return true when clicked (most
		                             // widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
		            1000.0f / ImGui::GetIO().Framerate,
		            ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window) {
		ImGui::Begin(
		  "Another Window",
		  &show_another_window); // Pass a pointer to our bool variable (the
		                         // window will have a closing button that will
		                         // clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}

void
DebugUI::RenderFrame()
{
	// Rendering
	ImGui::Render();
}

void
DebugUI::DrawFrame()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void
DebugUI::fullScreenWindowToggle()
{
	// returns an array of handles for all currently connected monitors
	// the primary monitor is always first in the returned array
	// if no monitors were found, this function returns NULL.

	GLFWmonitor* fullscreenMonitorPtr;

	if (fullScreenWindow) {
		this->monitors = glfwGetMonitors(&monitorCount);

		if (monitorCount > 0 && monitors[0] != NULL) {
			fullscreenMonitorPtr = monitors[fullScreenMonitor];

			const GLFWvidmode* mode = glfwGetVideoMode(fullscreenMonitorPtr);

			glfwSetWindowMonitor(parentWindow,
			                     fullScreenWindow ? fullscreenMonitorPtr : NULL,
			                     0,
			                     0,
			                     mode->width,
			                     mode->height,
			                     GLFW_DONT_CARE);
		}
	} else {
		glfwSetWindowMonitor(
		  parentWindow, NULL, 100, 100, 800, 600, GLFW_DONT_CARE);
	}

	// glfwGetMonitorPos(monitors[0], &mx, &my);
}