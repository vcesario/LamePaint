#include "ui.h"
#include "app.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

int STARTING_W = 800;
int STARTING_H = 600;

void OnCursorMoved_Callback(GLFWwindow* window, double xpos, double ypos);

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

	SetupCanvas(STARTING_W, STARTING_H);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// setup imgui renderers
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(/*"#version 130"*/);

	glViewport(0, 0, STARTING_W, STARTING_H);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();


		// imgui logic
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		vec2 pixelCoord = GetCursorPos_Pixel(); // this method uses imgui's frame info, so needs to be called after NewFrame()
		
		DrawMainMenu();
		DrawBottomBar(pixelCoord.x, pixelCoord.y, io.Framerate);

		// render
		glClear(GL_COLOR_BUFFER_BIT);

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