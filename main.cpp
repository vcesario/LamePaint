#include "ui.h"
#include "app.h"
#include "shader_s.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include "imgui/imgui.h"

const int STARTING_WINDOW_W = 800;
const int STARTING_WINDOW_H = 600;

double CursorX;
double CursorY;
double CursorX_LastFrame;
double CursorY_LastFrame;
bool IsClicking;
bool IsDragging;
bool ClickDownConsumed;

void OnCursorMoved_Callback(GLFWwindow* window, double xpos, double ypos);
void OnMouseClicked_Callback(GLFWwindow* window, int button, int action, int mods);

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(STARTING_WINDOW_W, STARTING_WINDOW_H, "Learn OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//glfwSwapInterval(0); // vsync off - useful to understand app performance even if above 60fps
	//glfwSwapInterval(1); // vsync on - needed for my personal notebook's screen, it seems

	glfwSetCursorPosCallback(window, OnCursorMoved_Callback);
	glfwSetMouseButtonCallback(window, OnMouseClicked_Callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	InitUI(window);
	ImGuiIO& io = ImGui::GetIO(); // wanted to do this inside InitUI() but I don't know how to properly make it global

	SetupCanvas(STARTING_WINDOW_W, STARTING_WINDOW_H);

	Shader shader("sh.vert", "sh.frag");
	shader.use();

	// setup texture and FBO
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set wrapping/filtering options on currently bound texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//int width, height, nrChannels;
	//unsigned char* data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);
	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	std::cout << "Failed to load texture" << std::endl;
	//	glfwTerminate();
	//	return -1;
	//}
	//stbi_image_free(data);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CanvasWidth, CanvasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	glGenerateMipmap(GL_TEXTURE_2D); // apparently this is mandatory or else the actual textures aren't generated? not sure
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, STARTING_W, STARTING_H, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//unsigned int fbo;
	//glGenFramebuffers(1, &fbo);
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//GLuint clearColor[4] = { 0, 0, 0, 0 };

	// setup screen quad
	float framePercent = (float)FrameHeight() / (float)STARTING_WINDOW_H;
	float canvasEdgeAnchor = (1 - framePercent) * 2 - 1;
	float vertices[] = {
		-1.0f,  canvasEdgeAnchor, 0.0f, 0.0f, 0.0f, // top left
		 1.0f,  canvasEdgeAnchor, 0.0f, 1.0f, 0.0f, // top right
		 1.0f, -canvasEdgeAnchor, 0.0f, 1.0f, 1.0f, // bottom right
		-1.0f, -canvasEdgeAnchor, 0.0f, 0.0f, 1.0f, // bottom left
	};
	int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	// initialize VAO + VBO + EBO
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// 1. bind VAO
	glBindVertexArray(VAO);
	// 2. copy vertices[] to VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. copy indices[] to EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 4. set VAO pointers
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	// tex coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glViewport(0, 0, STARTING_WINDOW_W, STARTING_WINDOW_H);
	glClearColor(0.3f, 0.7f, 0.1f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		// app logic
		vec2 pixelCoord = GetCursorPos_Pixel(CursorX, CursorY);
		if (IsClicking)
		{
			int brushSize = 3;
			if (IsDragging)
			{
				PaintRectangle(CursorX_LastFrame, CursorY_LastFrame, CursorX, CursorY, brushSize);
				PaintAtPixelCoord(CursorX, CursorY, brushSize);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CanvasWidth, CanvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
			}
			else if (!ClickDownConsumed)
			{
				PaintAtPixelCoord(CursorX, CursorY, brushSize);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CanvasWidth, CanvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

				ClickDownConsumed = true;
			}
		}

		// app render
		glClear(GL_COLOR_BUFFER_BIT);
		//glBindTexture(GL_TEXTURE_2D, texture);
		//shader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		DrawUI(pixelCoord.x, pixelCoord.y, io.Framerate);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// terminate imgui
	TerminateUI();

	glfwTerminate();
	return -1;
}

void OnCursorMoved_Callback(GLFWwindow* window, double xpos, double ypos)
{
	CursorX_LastFrame = CursorX;
	CursorY_LastFrame = CursorY;

	CursorX = xpos;
	CursorY = ypos;

	IsDragging = IsClicking;
	if (IsDragging)
	{
		ClickDownConsumed = true;
	}
}

void OnMouseClicked_Callback(GLFWwindow* window, int button, int action, int mods)
{
	//std::cout << button << ", " << action << ", " << mods << std::endl;
	if (button == 0)
	{
		IsClicking = action;

		if (action == 0)
		{
			ClickDownConsumed = false;
		}
	}
}