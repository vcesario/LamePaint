#include "ui.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

void DrawMainMenu();
void DrawBottomBar(int mouseX, int mouseY, float fps);
void DrawToolsWindow();

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

	DrawToolsWindow();

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

void DrawBottomBar(int posX, int posY, float fps)
{
	ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	float height = ImGui::GetFrameHeight();

	if (ImGui::BeginViewportSideBar("StatusBar", viewport, ImGuiDir_Down, height, window_flags))
	{
		if (ImGui::BeginMenuBar())
		{
			std::stringstream mousePosStream;
			mousePosStream << posX << ", " << posY << "px";
			ImGui::Text(mousePosStream.str().c_str());

			const float remainingWidth = ImGui::GetContentRegionAvail().x;

			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << fps << " FPS";

			std::string fpsText = stream.str();
			ImVec2 textSize = ImGui::CalcTextSize(fpsText.c_str());
			ImGui::Dummy({ remainingWidth - textSize.x - 8, height });
			ImGui::Text(fpsText.c_str());
			ImGui::EndMenuBar();
		}

		ImGui::End();
	}
}

const ImVec2 m_ToolsWindowSize(120, 250);
const ImVec2 m_ToolsWindowPos(650, 100);
static int m_BrushSize = 1;

void DrawToolsWindow()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;

	ImGui::Begin("Tools", NULL, window_flags);

	ImGui::SetWindowSize(m_ToolsWindowSize, ImGuiCond_Once);
	ImGui::SetWindowPos(m_ToolsWindowPos, ImGuiCond_Once);
	if (ImGui::Button("Round brush"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	{
		std::cout << "round brush selected" << std::endl;
	}
	if (ImGui::Button("Eraser"))
	{
		std::cout << "Eraser selected" << std::endl;
	}
	if (ImGui::Button("Paint bucket"))
	{
		std::cout << "paint bucket selected" << std::endl;
	}
	ImGui::SliderInt("Brush size", &m_BrushSize, 1, 100);

	ImVec2 button_sz(24, 24);
	ImGuiStyle& style = ImGui::GetStyle();
	int buttons_count = 6;
	float window_visible_x2 = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;
	for (int n = 0; n < buttons_count; n++)
	{
		ImGui::PushID(n);
		ImGui::Button("Box", button_sz);
		float last_button_x2 = ImGui::GetItemRectMax().x;
		float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
		if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
			ImGui::SameLine();
		ImGui::PopID();
	}
	ImGui::End();
}