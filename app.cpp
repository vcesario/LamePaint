#include "app.h"
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"

double m_CursorX;
double m_CursorY;

int m_PixelX;
int m_PixelY;

int m_WindowWidth;
int m_WindowHeight;

int CanvasWidth;
int CanvasHeight;


void UpdateCursorPos(double xpos, double ypos)
{
	m_CursorX = xpos;
	m_CursorY = ypos;

	m_PixelX = m_CursorX;
	int canvasTopEdge = ImGui::GetFrameHeight(); // first row of pixels that is part of canvas (0-based) = 19
	int canvasBottomEdge = m_WindowHeight - ImGui::GetFrameHeight(); // first row of pixels that is NOT part of canvas (0-based) = 581
	int canvasSize = canvasBottomEdge - canvasTopEdge;

	if (m_CursorY < canvasTopEdge)
	{
		m_PixelY = 0;
	}
	else if (m_CursorY >= canvasBottomEdge)
	{
		m_PixelY = canvasSize - 1;
	}
	else
	{
		m_PixelY = m_CursorY - canvasTopEdge;
	}
}

vec2 GetCursorPos_Pixel()
{
	//return vec2(m_CursorX, m_CursorY);
	return vec2(m_PixelX, m_PixelY);
}

void SetupCanvas(int width, int height)
{
	m_WindowWidth = width;
	m_WindowHeight = height;

	CanvasWidth = width;
	CanvasHeight = height - ImGui::GetFrameHeight() * 2;
}