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

#include <stb_image.h>

enum class GuiAnchor : int
{
	TL = 0,
	TR = 1,
	BL = 2,
	BR = 3,
	C = 4,
	TC = 5,
	BC = 6,
	LC = 7,
	RC = 8
};

class GuiWidget
{
public:
	GuiWidget(const char* filePath,
	          int         x_start,
	          int         y_start,
	          int         width,
	          int         height,
	          GuiAnchor   anchor,
	          GLFWwindow* gameWindow)
	  : filePath(filePath)
	  , x_start(x_start)
	  , y_start(y_start)
	  , width(width)
	  , height(height)
	  , anchor(anchor)
	  , gameWindow(gameWindow)
	{
		LoadImage();
		Init();
		isVisible = true;
	}

	~GuiWidget()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteProgram(shaderProgram);
	}

	void UpdateViewportData()
	{
		glfwGetFramebufferSize(gameWindow, &framebufferWidth, &framebufferHeight);

		// float alignment = framebufferWidth / 2;

		float pixel_x = static_cast<float>(x_start); //+alignment - (width / 2);
		float pixel_y = static_cast<float>(y_start);

		float x_ndc = 2.0f * (pixel_x + 0.5f) / framebufferWidth - 1.0f;
		float y_ndc = 2.0f * (pixel_y + 0.5f) / framebufferHeight - 1.0f;

		float x_end = 2.0f * (pixel_x + width + 0.5f) / framebufferWidth - 1.0f;
		float y_end = 2.0f * (pixel_y + height + 0.5f) / framebufferHeight - 1.0f;

		unsigned int indices[6] = {
			// note that we start from 0!
			0, 1, 3, // first Triangle
			1, 2, 3  // second Triangle
		};

		float vertices[] = {
			// positions          // colors           // texture coords
			x_end, y_end, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
			x_end, y_ndc, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
			x_ndc, y_ndc, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
			x_ndc, y_end, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
		};

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(
		  GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
		// (void*)0); glEnableVertexAttribArray(0);

		// position attribute
		glVertexAttribPointer(
		  0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(
		  1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(
		  2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	bool IsVisible() { return isVisible; }
	void SetVisible(bool visibility) { isVisible = visibility; }
	void Draw()
	{
		// note that this is allowed, the call to glVertexAttribPointer registered
		// VBO as the vertex attribute's bound vertex buffer object so afterwards we
		// can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// remember: do NOT unbind the EBO while a VAO is active as the bound
		// element buffer object IS stored in the VAO; keep the EBO bound.
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// You can unbind the VAO afterwards so other VAO calls won't accidentally
		// modify this VAO, but this rarely happens. Modifying other VAOs requires a
		// call to glBindVertexArray anyways so we generally don't unbind VAOs (nor
		// VBOs) when it's not directly necessary.
		glBindVertexArray(0);

		glUseProgram(shaderProgram);
		glBindTexture(GL_TEXTURE_2D, texture-1);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

private:
	const char* filePath;
	int         x_start;
	int         y_start;
	int         width;
	int         height;
	GuiAnchor   anchor;
	bool        isVisible;
	int         nrChannels;

	int readWidth;
	int readHeight;

	int framebufferWidth;
	int framebufferHeight;

	unsigned int texture;

	GLFWwindow* gameWindow;

	void LoadImage()
	{
		// load image, create texture and generate mipmaps
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data =
		  stbi_load(filePath, &readWidth, &framebufferHeight, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D,
			             0,
			             GL_RGBA,
			             readWidth,
			             framebufferHeight,
			             0,
			             GL_RGBA,
			             GL_UNSIGNED_BYTE,
			             data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		} else {
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_set_flip_vertically_on_load(false);
	}

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

	int shaderProgram;

	unsigned int VBO, VAO, EBO;

	void Init()
	{
		glfwGetFramebufferSize(gameWindow, &framebufferWidth, &framebufferHeight);

		//float alignment = framebufferWidth / 2;

		float pixel_x = static_cast<float>(x_start);//+alignment - (width / 2);
		float pixel_y = static_cast<float>(y_start);

		float x_ndc = 2.0f * (pixel_x + 0.5f) / framebufferWidth - 1.0f;
		float y_ndc = 2.0f * (pixel_y + 0.5f) / framebufferHeight - 1.0f;

		float x_end = 2.0f * (pixel_x + width + 0.5f) / framebufferWidth - 1.0f;
		float y_end = 2.0f * (pixel_y + height + 0.5f) / framebufferHeight - 1.0f;

		unsigned int indices[6] = {
			// note that we start from 0!
			0, 1, 3, // first Triangle
			1, 2, 3  // second Triangle
		};

		float vertices[] = {
			// positions          // colors           // texture coords
			x_end, y_end, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
			x_end, y_ndc, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
			x_ndc, y_ndc, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
			x_ndc, y_end, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
		};

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
		shaderProgram = glCreateProgram();
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

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s),
		// and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(
		  GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
		// (void*)0); glEnableVertexAttribArray(0);

		// position attribute
		glVertexAttribPointer(
		  0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
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
	}
};

class GuiManager
{
public:
	GuiManager() = default;
	~GuiManager() {}
	void Draw()
	{
		glDisable(GL_DEPTH_TEST);

		for (auto const& [label, widget] : widgets) {
			if (widget->IsVisible()) {
				widget->Draw();
			}
		}

		glEnable(GL_DEPTH_TEST);
	}

	void UpdateViewportData()
	{
		for (auto const& [label, widget] : widgets) {
			widget->UpdateViewportData();
		}
	}

	void AddWidget(const char* label, std::shared_ptr<GuiWidget> widget)
	{
		widgets.insert(std::make_pair(label, widget));
	}

private:
	std::map<const char*, std::shared_ptr<GuiWidget>> widgets;
};

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

	void ShowWindow();

	void CloseSplashScreen();

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

	template<typename Function>
	void SetFramebufferSizeCallback(Function framebuffer_size_callback);

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

	std::shared_ptr<GuiManager> guiManager;

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

	void        SplashScreen(GLFWwindow* mainWindow);
	GLFWwindow* splashScreen;
};