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

	Settings settingsFromFile = ConfigManager::GetSettings();

	borderlessWindow = settingsFromFile.borderless;
	fullScreenWindow =
	  settingsFromFile
	    .fullscreen; // FIXME: fullscreen doesn't work at start of the game
	resizableWindow = settingsFromFile.resizable;
	vsyncEnabled = settingsFromFile.vsync;
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

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct DebugUI::AppLog
{
	ImGuiTextBuffer Buf;
	ImGuiTextFilter Filter;
	ImVector<int>
	  LineOffsets;   // Index to lines offset. We maintain this with AddLog()
	                 // calls, allowing us to have a random access on lines
	bool AutoScroll; // Keep scrolling if already at the bottom

	AppLog()
	{
		AutoScroll = true;
		Clear();
	}

	void Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int     old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
	}

	void Draw(const char* title, bool* p_open = NULL)
	{
		if (!ImGui::Begin(title, p_open)) {
			ImGui::End();
			return;
		}

		// Options menu
		if (ImGui::BeginPopup("Options")) {
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}

		// Main window
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);

		ImGui::Separator();
		ImGui::BeginChild(
		  "scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		if (clear)
			Clear();
		if (copy)
			ImGui::LogToClipboard();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		const char* buf = Buf.begin();
		const char* buf_end = Buf.end();
		if (Filter.IsActive()) {
			// In this example we don't use the clipper when Filter is enabled.
			// This is because we don't have a random access on the result on our
			// filter. A real application processing logs with ten of thousands of
			// entries may want to store the result of search/filter. especially if
			// the filtering function is not trivial (e.g. reg-exp).
			for (int line_no = 0; line_no < LineOffsets.Size; line_no++) {
				const char* line_start = buf + LineOffsets[line_no];
				const char* line_end = (line_no + 1 < LineOffsets.Size)
				                         ? (buf + LineOffsets[line_no + 1] - 1)
				                         : buf_end;
				if (Filter.PassFilter(line_start, line_end))
					ImGui::TextUnformatted(line_start, line_end);
			}
		} else {
			// The simplest and easy way to display the entire buffer:
			//   ImGui::TextUnformatted(buf_begin, buf_end);
			// And it'll just work. TextUnformatted() has specialization for large
			// blob of text and will fast-forward to skip non-visible lines. Here we
			// instead demonstrate using the clipper to only process lines that are
			// within the visible area. If you have tens of thousands of items and
			// their processing cost is non-negligible, coarse clipping them on your
			// side is recommended. Using ImGuiListClipper requires A) random access
			// into your data, and B) items all being the  same height, both of
			// which we can handle since we an array pointing to the beginning of
			// each line of text. When using the filter (in the block of code above)
			// we don't have random access into the data to display anymore, which
			// is why we don't use the clipper. Storing or skimming through the
			// search result would make it possible (and would be recommended if you
			// want to search through tens of thousands of entries)
			ImGuiListClipper clipper;
			clipper.Begin(LineOffsets.Size);
			while (clipper.Step()) {
				for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd;
				     line_no++) {
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size)
					                         ? (buf + LineOffsets[line_no + 1] - 1)
					                         : buf_end;
					ImGui::TextUnformatted(line_start, line_end);
				}
			}
			clipper.End();
		}
		ImGui::PopStyleVar();

		if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
		ImGui::End();
	}
};

// Demonstrate creating a simple log window with basic filtering.
void
DebugUI::ShowAppLog(bool* p_open)
{
	static AppLog log;

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
			log.AddLog(
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
	log.Draw("Debug Log", p_open);
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

		ImGui::EndMainMenuBar();
	}

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