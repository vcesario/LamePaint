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

void DrawMainMenu(GLFWwindow* window);
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

void DrawUI(GLFWwindow* window, int pixelX, int pixelY, float framerate, TextureObject iconTex)
{
	// imgui logic
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	DrawMainMenu(window);
	DrawBottomBar(pixelX, pixelY, framerate);

	DrawToolsWindow(iconTex);

	// imgui render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DrawMainMenu(GLFWwindow* window)
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
				glfwSetWindowShouldClose(window, true);
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

	ImVec2 imgSize = ImVec2(24.5f, 24.5f);
	float imgPadding = (ImGui::GetContentRegionAvail().x - imgSize.x) / 2;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(imgPadding, 0));

	ImVec2 uvMin = ImVec2(imgSize.x * 1 / 98.0f, imgSize.y * 3 / 98.0f);
	ImVec2 uvMax = ImVec2(imgSize.x * 2 / 98.0f, imgSize.y * 4 / 98.0f);
	if (ImGui::ImageButton("##RoundBrush", (void*)(intptr_t)iconTex.id, imgSize, uvMin, uvMax))
	{
		//std::cout << "round brush selected" << std::endl;
		SetModeToDefault();
		SetUIBrushSlider(GetBrushSize());
	}

	uvMin = ImVec2(imgSize.x * 0 / 98.0f, imgSize.y * 1 / 98.0f);
	uvMax = ImVec2(imgSize.x * 1 / 98.0f, imgSize.y * 2 / 98.0f);
	if (ImGui::ImageButton("##Eraser", (void*)(intptr_t)iconTex.id, imgSize, uvMin, uvMax))
	{
		//std::cout << "Eraser selected" << std::endl;
		SetModeToEraser();
		SetUIBrushSlider(GetBrushSize());
	}

	uvMin = ImVec2(imgSize.x * 1 / 98.0f, imgSize.y * 1 / 98.0f);
	uvMax = ImVec2(imgSize.x * 2 / 98.0f, imgSize.y * 2 / 98.0f);
	if (ImGui::ImageButton("##PaintBucket", (void*)(intptr_t)iconTex.id, imgSize, uvMin, uvMax))
	{
		//std::cout << "paint bucket selected" << std::endl;
		SetModeToBucket();
		SetUIBrushSlider(GetBrushSize());
	}
	ImGui::PopStyleVar();

	char sliderLabel[] = "px\0";
	float sliderWidth = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(sliderLabel).x;
	ImGui::SetNextItemWidth(sliderWidth);
	if (ImGui::SliderInt(sliderLabel, &m_BrushSize, 1, 100))
	{
		SetBrushSize(m_BrushSize);
	}

	float buttonWidth = ImGui::GetContentRegionAvail().x / 2.0f - ImGui::GetStyle().FramePadding.x;
	ImVec2 colorButtonSize(buttonWidth, 20);

	int count = 0;
	for (auto const& colorPair : m_ColorIdToVal)
	{
		ImVec4 color01 = ImVec4(colorPair.second.x / 255.0f, colorPair.second.y / 255.0f, colorPair.second.z / 255.0f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, color01);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color01);
		ImGui::PushID(count);
		if (ImGui::Button("", colorButtonSize))
		{
			SetBrushColor(colorPair.first);
		}
		ImGui::PopID();
		ImGui::PopStyleColor(2);
		
		if (++count % 2 == 1)
		{
			ImGui::SameLine();
		}
	}

	ImGui::End();
}

void SetUIBrushSlider(int value)
{
	m_BrushSize = value;
}

bool IsCursorHoveringUI()
{
	return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
}