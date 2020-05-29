#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "DebugUI.hpp"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a
// merged icon fonts (see docs/FONTS.txt)
static void
HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

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

	Settings settingsFromFile = ConfigManager::GetSettings();

	borderlessWindow = settingsFromFile.borderless;
	fullScreenWindow =
	  settingsFromFile
	    .fullscreen; // FIXME: fullscreen doesn't work at start of the game
	resizableWindow = settingsFromFile.resizable;
	vsyncEnabled = settingsFromFile.vsync;

	log = new AppLog;
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

// Demonstrate creating a simple log window with basic filtering.
void
DebugUI::ShowAppLog(bool* p_open)
{
	//static AppLog log;

	// For the demo: add a debug button _BEFORE_ the normal log window contents
	// We take advantage of a rarely used feature: multiple calls to
	// Begin()/End() are appending to the _same_ window. Most of the contents of
	// the window will be added by the log.Draw() call.
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin("Debug Log", p_open);
	if (ImGui::SmallButton("[Debug] Add 5 entries")) {
		static int counter = 0;
		for (int n = 0; n < 5; n++) {
			const char* categories[3] = { "info", "warn", "error" };
			const char* words[] = { "Bumfuzzled",    "Cattywampus",  "Snickersnee",
				                      "Abibliophobia", "Absquatulate", "Nincompoop",
				                      "Pauciloquent" };
			log->AddLog(
			  "[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
			  ImGui::GetFrameCount(),
			  categories[counter % IM_ARRAYSIZE(categories)],
			  ImGui::GetTime(),
			  words[counter % IM_ARRAYSIZE(words)]);
			counter++;
		}
	}
	ImGui::End();

	// Actually call in the regular Log helper (which will Begin() into the same
	// window as we just did)
	log->Draw("Debug Log", p_open);
}

void
DebugUI::Overlay()
{
	if (!showOverlay) {
		return;
	}

	const float DISTANCE = 10.0f;
	static int  corner = 2;
	ImGuiIO&    io = ImGui::GetIO();
	if (corner != -1) {
		ImVec2 window_pos =
		  ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE,
		         (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot =
		  ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin(
	      "Info box",
	      std::addressof(showOverlay),
	      (corner != -1 ? ImGuiWindowFlags_NoMove : 0) |
	        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
	        ImGuiWindowFlags_NoSavedSettings |
	        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
		//ImGui::Text("(right-click to change position)");
		ImGui::Text("Info box\t\t\t\t\t");
		ImGui::SameLine();
		HelpMarker("right-click to change position");
		ImGui::Separator();
		/*ImGui::Text("- or me");
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("I am a fancy tooltip");
			static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
			ImGui::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
			ImGui::EndTooltip();
		}*/
		//ImGui::Separator();
		ImGui::Text("Application average \n%.3f ms/frame (%.1f FPS)",
		            1000.0f / ImGui::GetIO().Framerate,
		            ImGui::GetIO().Framerate);
		ImGui::Separator();
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse Position: <invalid>");
		if (ImGui::BeginPopupContextWindow()) {
			if (ImGui::MenuItem("Custom", NULL, corner == -1))
				corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0))
				corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1))
				corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2))
				corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3))
				corner = 3;
			if (std::addressof(showOverlay) && ImGui::MenuItem("Close"))
				showOverlay = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void
DebugUI::SetupWidgets()
{
	//bool show_demo_window = true;
	//bool show_another_window = true;

	glfwSwapInterval(static_cast<int>(vsyncEnabled));

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Window")) {

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

		if (ImGui::BeginMenu("Msg Log")) {
			if (ImGui::MenuItem("Display", NULL, this->isLogDisplayed)) {
				isLogDisplayed = !isLogDisplayed;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Widgets")) {
			if (ImGui::MenuItem("Info overlay", NULL, this->showOverlay)) {
				showOverlay = !showOverlay;
			}
			if (ImGui::MenuItem("ImGui demo window", NULL, this->showDemoWindow)) {
				showDemoWindow = !showDemoWindow;
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Quit")) {
			glfwSetWindowShouldClose(parentWindow, true);
		}

		ImGui::EndMainMenuBar();
	}

	Overlay();

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// 1. Show the big demo window (Most of the sample code is in
	// ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
	// ImGui!).
	//if (show_demo_window)
	//	ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair
	// to created a named window.
	/*{
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
	}*/

	// 3. Show another simple window.
	/*if (show_another_window) {
		ImGui::Begin(
		  "Another Window",
		  &show_another_window); // Pass a pointer to our bool variable (the
		                         // window will have a closing button that will
		                         // clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}*/

	if (this->isLogDisplayed) {
		ShowAppLog(&isLogDisplayed);
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