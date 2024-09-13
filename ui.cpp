#include "ui.h"
#include "app.h"

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
void DrawToolsWindow(TextureObject iconTex);

const ImVec2 m_ToolsWindowSize(150, 250);
const ImVec2 m_ToolsWindowPos(800 - m_ToolsWindowSize.x - 20, 100);
const ImVec2 m_ColorButtonSize(m_ToolsWindowSize.x / 2.0f, 24);
static int m_BrushSize = 5;

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

void DrawUI(int pixelX, int pixelY, float framerate, TextureObject iconTex)
{
	// imgui logic
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	DrawMainMenu();
	DrawBottomBar(pixelX, pixelY, framerate);

	DrawToolsWindow(iconTex);

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

void DrawToolsWindow(TextureObject iconTex)
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;

	ImGui::Begin("Tools", NULL, window_flags);

	ImGui::SetWindowSize(m_ToolsWindowSize, ImGuiCond_Once);
	ImGui::SetWindowPos(m_ToolsWindowPos, ImGuiCond_Once);

	ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // No tint
	ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
	ImGui::Image((void*)(intptr_t)iconTex.id, ImVec2(iconTex.width, iconTex.height), uv_min, uv_max, tint_col, border_col);

	ImVec2 wideButtonSize(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight());

	if (ImGui::Button("Round brush", wideButtonSize))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	{
		//std::cout << "round brush selected" << std::endl;
		SetModeToDefault();
		SetUIBrushSlider(GetBrushSize());
	}
	if (ImGui::Button("Eraser", wideButtonSize))
	{
		//std::cout << "Eraser selected" << std::endl;
		SetModeToEraser();
		SetUIBrushSlider(GetBrushSize());
	}
	if (ImGui::Button("Paint bucket", wideButtonSize))
	{
		std::cout << "paint bucket selected" << std::endl;
	}

	char sliderLabel[] = "px\0";
	float sliderWidth = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(sliderLabel).x;
	ImGui::SetNextItemWidth(sliderWidth);
	if (ImGui::SliderInt(sliderLabel, &m_BrushSize, 1, 100))
	{
		SetBrushSize(m_BrushSize);
	}

	float buttonWidth = ImGui::GetContentRegionAvail().x / 2.0f - ImGui::GetStyle().FramePadding.x;
	ImVec2 colorButtonSize(buttonWidth, 24);

	vec3byte nextColor = m_ColorIdToVal[Colors::Black];
	ImVec4 nextColor01 = ImVec4(nextColor.x / 255.0f, nextColor.y / 255.0f, nextColor.z / 255.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, nextColor01);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, nextColor01);
	if (ImGui::Button("1", colorButtonSize))
	{
		SetBrushColor(Colors::Black);
	}
	ImGui::PopStyleColor(2);

	ImGui::SameLine();

	nextColor = m_ColorIdToVal[Colors::White];
	nextColor01 = ImVec4(nextColor.x / 255.0f, nextColor.y / 255.0f, nextColor.z / 255.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, nextColor01);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, nextColor01);
	if (ImGui::Button("2", colorButtonSize))
	{
		SetBrushColor(Colors::White);
	}
	ImGui::PopStyleColor(2);

	nextColor = m_ColorIdToVal[Colors::Red];
	nextColor01 = ImVec4(nextColor.x / 255.0f, nextColor.y / 255.0f, nextColor.z / 255.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, nextColor01);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, nextColor01);
	if (ImGui::Button("3", colorButtonSize))
	{
		SetBrushColor(Colors::Red);
	}
	ImGui::PopStyleColor(2);

	ImGui::SameLine();

	nextColor = m_ColorIdToVal[Colors::Green];
	nextColor01 = ImVec4(nextColor.x / 255.0f, nextColor.y / 255.0f, nextColor.z / 255.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, nextColor01);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, nextColor01);
	if (ImGui::Button("4", colorButtonSize))
	{
		SetBrushColor(Colors::Green);
	}
	ImGui::PopStyleColor(2);

	nextColor = m_ColorIdToVal[Colors::Blue];
	nextColor01 = ImVec4(nextColor.x / 255.0f, nextColor.y / 255.0f, nextColor.z / 255.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, nextColor01);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, nextColor01);
	if (ImGui::Button("5", colorButtonSize))
	{
		SetBrushColor(Colors::Blue);
	}
	ImGui::PopStyleColor(2);

	ImGui::SameLine();

	nextColor = m_ColorIdToVal[Colors::Yellow];
	nextColor01 = ImVec4(nextColor.x / 255.0f, nextColor.y / 255.0f, nextColor.z / 255.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, nextColor01);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, nextColor01);
	if (ImGui::Button("6", colorButtonSize))
	{
		SetBrushColor(Colors::Yellow);
	}
	ImGui::PopStyleColor(2);

	ImGui::End();
}

void SetUIBrushSlider(int value)
{
	m_BrushSize = value;
}