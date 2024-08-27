#include "app.h"
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"

double m_CursorX;
double m_CursorY;

int m_WindowWidth;
int m_WindowHeight;

void UpdateCursorPos(double xpos, double ypos)
{
	m_CursorX = xpos;
	m_CursorY = ypos;
}

vec2 GetCursorPos_Pixel()
{
	int canvasTopEdge = ImGui::GetFrameHeight(); // first row of pixels that is part of canvas (0-based) = 19
	int canvasBottomEdge = m_WindowHeight - ImGui::GetFrameHeight(); // first row of pixels that is NOT part of canvas (0-based) = 581
	int canvasSize = canvasBottomEdge - canvasTopEdge;

	float pixelY = m_CursorY;
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

void SetupCanvas(int width, int height)
{
	m_WindowWidth = width;
	m_WindowHeight = height;
}