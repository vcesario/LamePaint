#include "app.h"
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"

double m_CursorX;
double m_CursorY;

void UpdateCursorPos(double xpos, double ypos)
{
	m_CursorX = xpos;
	m_CursorY = ypos;
}

vec2 GetCursorPos_Pixel()
{
	int canvasTopEdge = ImGui::GetFrameHeight();
	int canvasBottomEdge = ImGui::GetWindowHeight() - ImGui::GetFrameHeight();
	int canvasSize = canvasBottomEdge - canvasTopEdge;

	float pixelY = m_CursorY - ImGui::GetFrameHeight();
	if (m_CursorY < canvasTopEdge)
	{
		pixelY = 0;
	}
	else if (m_CursorY >= canvasBottomEdge)
	{
		pixelY = canvasSize - 1;
	}
	else
	{
		pixelY = m_CursorY - canvasTopEdge;
	}

	return vec2(m_CursorX, pixelY);
}