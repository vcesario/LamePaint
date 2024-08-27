#include "ui.h"
#include "app.h"
#include "shader_s.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

const int STARTING_W = 800;
const int STARTING_H = 600;

void OnCursorMoved_Callback(GLFWwindow* window, double xpos, double ypos);
void OnMouseClicked_Callback(GLFWwindow* window, int button, int action, int mods);

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(STARTING_W, STARTING_H, "Learn OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//glfwSwapInterval(0); // vsync off - useful to understand app performance even if above 60fps

	glfwSetCursorPosCallback(window, OnCursorMoved_Callback);
	glfwSetMouseButtonCallback(window, OnMouseClicked_Callback);

	SetupCanvas(STARTING_W, STARTING_H);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

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

	std::vector<GLubyte> data(STARTING_W * STARTING_H * 4, 200);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, STARTING_W, STARTING_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	glGenerateMipmap(GL_TEXTURE_2D); // apparently this is mandatory or else the actual textures aren't generated? not sure
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, STARTING_W, STARTING_H, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	//unsigned int fbo;
	//glGenFramebuffers(1, &fbo);
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//GLuint clearColor[4] = { 0, 0, 0, 0 };

	// setup screen quad
	float vertices[] = {
		-0.8f, -0.8f, 0.0f, 0.0f, 1.0f, // top left
		 0.8f, -0.8f, 0.0f, 1.0f, 1.0f, // top right
		 0.8f,  0.8f, 0.0f, 1.0f, 0.0f, // bottom right
		-0.8f,  0.8f, 0.0f, 0.0f, 0.0f, // bottom left
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

	// setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// setup imgui renderers
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(/*"#version 130"*/);

	glViewport(0, 0, STARTING_W, STARTING_H);
	glClearColor(0.3f, 0.7f, 0.1f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// app logic
		vec2 pixelCoord = GetCursorPos_Pixel();

		// app render
		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, texture);
		shader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// imgui logic
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		DrawMainMenu();
		DrawBottomBar(pixelCoord.x, pixelCoord.y, io.Framerate);

		// imgui render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
	
	// terminate imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return -1;
}

void OnCursorMoved_Callback(GLFWwindow* window, double xpos, double ypos)
{
	UpdateCursorPos(xpos, ypos);
}

void OnMouseClicked_Callback(GLFWwindow* window, int button, int action, int mods)
{
	std::cout << button << ", " << action << ", " << mods << std::endl;
}