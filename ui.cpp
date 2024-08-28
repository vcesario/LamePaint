#include "ui.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <string>
#include <sstream>
#include <iomanip>

void DrawMainMenu();
void DrawBottomBar(int mouseX, int mouseY, float fps);

void InitUI(GLFWwindow* window)
{
	// setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// setup imgui renderers
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(/*"#version 130"*/);
}

void TerminateUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void DrawUI(int pixelX, int pixelY, float framerate)
{
	// imgui logic
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	DrawMainMenu();
	DrawBottomBar(pixelX, pixelY, framerate);

	// imgui render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DrawMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
			}

			if (ImGui::MenuItem("Open..."))
			{
			}

			if (ImGui::MenuItem("Export..."))
			{
			}

			if (ImGui::MenuItem("Exit"))
			{
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void DrawBottomBar(int mouseX, int mouseY, float fps)
{
	ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	float height = ImGui::GetFrameHeight();

	if (ImGui::BeginViewportSideBar("StatusBar", viewport, ImGuiDir_Down, height, window_flags))
	{
		if (ImGui::BeginMenuBar())
		{
			std::stringstream mousePosStream;
			mousePosStream << mouseX << ", " << mouseY << "px";
			ImGui::Text(mousePosStream.str().c_str());

			const float remainingWidth = ImGui::GetContentRegionAvail().x;

			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << fps << " FPS";

			std::string fpsText = stream.str();
			ImVec2 textSize = ImGui::CalcTextSize(fpsText.c_str());
			ImGui::Dummy({remainingWidth - textSize.x - 8, height});
			ImGui::Text(fpsText.c_str());
			ImGui::EndMenuBar();
		}

		ImGui::End();
	}
}