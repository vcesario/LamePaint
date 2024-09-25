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
#include <ShObjIdl.h>
#include <Windows.h>
#include <string>

void DrawMainMenu(GLFWwindow* window);
void DrawBottomBar(int mouseX, int mouseY, float fps);
void DrawToolsWindow(TextureObject iconTex);
bool openFile(std::string& filePath, std::string& selectedFile);

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
	static bool clearPopup = false;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Clear canvas..."))
			{
				clearPopup = true;
			}

			if (ImGui::MenuItem("Open..."))
			{
				std::string selectedFile;
				std::string filePath;
				bool result = openFile(filePath, selectedFile);

				if (!result)
				{
					std::cout << "ENCOUNTERED AN ERROR: " << GetLastError() << std::endl;
					return;
				}

				//std::cout << "SELECTED FILE: " << selectedFile.c_str() << std::endl;
					//std::cout << "FILE PATH: " << filePath.c_str() << std::endl;

				GLuint texId = 0;
				int texWidth = 0;
				int texHeight = 0;
				result = LoadTextureFromFile(filePath.c_str(), &texId, &texWidth, &texHeight);
				
				if (!result)
				{
					std::cout << "ENCOUNTERED AN ERROR: " << GetLastError() << std::endl;
					return;
				}

				TextureObject loadedImgTex(texId, texWidth, texHeight);
				TransferTexToCanvas(loadedImgTex);
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

	if (clearPopup)
	{
		ImGui::OpenPopup("Clear?");
		clearPopup = false;
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Clear?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("This beautiful drawing will be discarded. Proceed?");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			ClearCanvas();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
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


COMDLG_FILTERSPEC rgSpec[] =
{
	{ L"Image Files", L"*.jpg;*.jpeg;*.png;*.bmp" },
	//{ L"szBMP", L"*.bmp" },
	//{ L"szAll", L"*.*" },
};
bool openFile(std::string& filePath, std::string& selectedFile) // thanks https://stackoverflow.com/a/72429080
{
	//  CREATE FILE OBJECT INSTANCE
	HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(f_SysHr))
		return FALSE;

	// CREATE FileOpenDialog OBJECT
	IFileOpenDialog* f_FileSystem;
	f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
	if (FAILED(f_SysHr)) {
		CoUninitialize();
		return FALSE;
	}

	f_FileSystem->SetFileTypes(1, rgSpec);

	//  SHOW OPEN FILE DIALOG WINDOW
	f_SysHr = f_FileSystem->Show(NULL);
	if (FAILED(f_SysHr)) {
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}

	//  RETRIEVE FILE NAME FROM THE SELECTED ITEM
	IShellItem* f_Files;
	f_SysHr = f_FileSystem->GetResult(&f_Files);
	if (FAILED(f_SysHr)) {
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}

	//  STORE AND CONVERT THE FILE NAME
	PWSTR f_Path;
	f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
	if (FAILED(f_SysHr)) {
		f_Files->Release();
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}

	//  FORMAT AND STORE THE FILE PATH
	std::wstring path(f_Path);
	std::string c(path.begin(), path.end());
	filePath = c;

	//  FORMAT STRING FOR EXECUTABLE NAME
	const size_t slash = filePath.find_last_of("/\\");
	selectedFile = filePath.substr(slash + 1);

	//  SUCCESS, CLEAN UP
	CoTaskMemFree(f_Path);
	f_Files->Release();
	f_FileSystem->Release();
	CoUninitialize();
	return TRUE;
}