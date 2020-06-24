#include <clocale>
#include <iostream>
#include <stdio.h>

#include "Window.hpp"

#include <stb_image.h>

#include "VirtualInputManager.hpp"

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
	Window* handler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	handler->guiManager->UpdateViewportData();
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

	VirtualInputManager* vim = VirtualInputManager::GetInstance();
	vim->Update(button, InputSource::MOUSE_BUTTON, (float)action);
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

template<typename Function>
void
Window::SetFramebufferSizeCallback(Function framebuffer_size_callback)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

	VirtualInputManager* vim = VirtualInputManager::GetInstance();
	vim->Update(key, InputSource::KEY, (float)action);
}

Window* Window::mainWindowPtr = nullptr;

class Tmp
{
public:
	double beforeState = -1;
	double currentState = 0;
	void   execute(double data)
	{
		std::cout << "Call from TMP"
		          << "\n";
		beforeState = currentState;
		currentState = data;
	}
};

void
Window::SplashScreen(GLFWwindow* mainWindow)
{
	/*const char* vertexShaderSource =
	  "#version 330 core\n"
	  "layout (location = 0) in vec3 aPos;\n"
	  "void main()\n"
	  "{\n"
	  "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	  "}\0";
	const char* fragmentShaderSource =
	  "#version 330 core\n"
	  "out vec4 FragColor;\n"
	  "void main()\n"
	  "{\n"
	  "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	  "}\n\0";*/

	const char* vertexShaderSource =
	  "#version 330 core\n"
	  "layout(location = 0) in vec3 aPos;\n"
	  "layout(location = 1) in vec3 aColor;\n"
	  "layout(location = 2) in vec2 aTexCoord;\n"
	  "out vec3 ourColor;\n"
	  "out vec2 TexCoord;\n"
	  "void main()\n"
	  "{\n"
	  "	gl_Position = vec4(aPos, 1.0);\n"
	  "	ourColor = aColor;\n"
	  "	TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
	  "}\n\0";

	const char* fragmentShaderSource =
	  "#version 330 core\n"
	  "out vec4 FragColor;\n"
	  "in vec3 ourColor;\n"
	  "in vec2 TexCoord;\n"
	  "uniform sampler2D texture1;\n"
	  "void main()\n"
	  "{\n"
	  "	FragColor = texture(texture1, TexCoord);\n"
	  "}\n\0";

	int splashWidth = 400;
	int splashHeight = 250;

	// Get the resolution of the primary monitor
	const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	splashScreen =
	  glfwCreateWindow(splashWidth, splashHeight, "Splash Screen", NULL, NULL);

	glfwSetWindowAttrib(splashScreen, GLFW_DECORATED, GLFW_FALSE);
	glfwSetWindowAttrib(splashScreen, GLFW_RESIZABLE, GLFW_FALSE);

	glfwSetWindowPos(splashScreen,
	                 (vidmode->width - splashWidth) / 2,
	                 (vidmode->height - splashHeight) / 2);

	glfwMakeContextCurrent(splashScreen);
	glfwSwapInterval(1); // Enable vsync

	glClearColor(0.45f, 0.55f, 0.60f, 0.00f);
	glViewport(0, 0, splashWidth, splashHeight);

	glClear(GL_COLOR_BUFFER_BIT);

	glfwShowWindow(splashScreen);

	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
		          << infoLog << std::endl;
	}
	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
		          << infoLog << std::endl;
	}
	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
		          << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	/*float vertices[] = {
	  1.0f,  1.0f,  0.0f, // top right
	  1.0f,  -1.0f, 0.0f, // bottom right
	  -1.0f, -1.0f, 0.0f, // bottom left
	  -1.0f, 1.0f,  0.0f  // top left
	};*/
	float vertices[] = {
		// positions          // colors           // texture coords
		1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
		1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
	};
	unsigned int indices[] = {
		// note that we start from 0!
		0, 1, 3, // first Triangle
		1, 2, 3  // second Triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and
	// then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
	  GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
	// (void*)0); glEnableVertexAttribArray(0);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(
	  1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(
	  2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// load and create a texture
	// -------------------------
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D,
	              texture); // all upcoming GL_TEXTURE_2D operations now have
	                        // effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(
	  GL_TEXTURE_2D,
	  GL_TEXTURE_WRAP_S,
	  GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can
	// find files on any IDE/platform; replace it with your own image path.
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data =
	  stbi_load("assets/images/splash.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D,
		             0,
		             GL_RGBA,
		             width,
		             height,
		             0,
		             GL_RGBA,
		             GL_UNSIGNED_BYTE,
		             data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_set_flip_vertically_on_load(false);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO
	// as the vertex attribute's bound vertex buffer object so afterwards we can
	// safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element
	// buffer object IS stored in the VAO; keep the EBO bound.
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally
	// modify this VAO, but this rarely happens. Modifying other VAOs requires a
	// call to glBindVertexArray anyways so we generally don't unbind VAOs (nor
	// VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// render
	// ------
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw our first triangle
	glUseProgram(shaderProgram);
	glBindVertexArray(
	  VAO); // seeing as we only have a single VAO there's no need to bind it
	        // every time, but we'll do so to keep things a bit more organized
	// glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	// glBindVertexArray(0); // no need to unbind it every time

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
	// etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(splashScreen);

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	// glfwSwapBuffers(splashScreen);

	// set the current context back to main window
	glfwMakeContextCurrent(mainWindow);
}

void
Window::CloseSplashScreen()
{
	// glfwHideWindow(splashScreen);
	glfwDestroyWindow(splashScreen);
}

Window::Window(std::string windowTitle)
  : windowTitle(windowTitle)
{
	input = std::make_shared<InputManager>();

	guiManager = std::make_shared<GuiManager>();

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

		SplashScreen(window);

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
		SetFramebufferSizeCallback(DefaultFramebufferSizeCallback);

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
	glfwWindowHint(GLFW_SAMPLES, 4);                               // <--- to be changed from setting files
#endif
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
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

	// Get the resolution of the primary monitor
	const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	// Center our window
	glfwSetWindowPos(window,
	                 (vidmode->width - windowSettings.size[0]) / 2,
	                 (vidmode->height - windowSettings.size[1]) / 2);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	return 0;
}

void
Window::ShowWindow()
{
	glfwMakeContextCurrent(window);
	// Make the window visible
	glfwShowWindow(window);
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
	if (framebufferWidth > 0) {
		return framebufferWidth;
	} else {
		return 1;
	}
}

int
Window::GetWindowHeight()
{
	if (framebufferHeight > 0) {
		return framebufferHeight;
	} else {
		return 1;
	}
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