#include "app.h"
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include <vector>

double m_CursorX;
double m_CursorY;

int m_PixelX;
int m_PixelY;

int m_WindowWidth;
int m_WindowHeight;

int CanvasWidth;
int CanvasHeight;

std::vector<GLubyte> data;

int Map(int a1, int b1, int val1, int a2, int b2)
{

}

void UpdateCursorPos(double xpos, double ypos)
{
	m_CursorX = xpos;
	m_CursorY = ypos;

	m_PixelX = m_CursorX;
	int canvasTopEdge = ImGui::GetFrameHeight(); // first row of pixels that is part of canvas (0-based) = 19
	//int canvasBottomEdge = m_WindowHeight - ImGui::GetFrameHeight(); // first row of pixels that is NOT part of canvas (0-based) = 581
	//int canvasSize = canvasBottomEdge - canvasTopEdge;

	//if (m_CursorY < canvasTopEdge)
	//{
	//	m_PixelY = 0;
	//}
	//else if (m_CursorY >= canvasBottomEdge)
	//{
	//	m_PixelY = canvasSize - 1;
	//}
	//else
	//{
		//m_PixelY = m_CursorY - canvasTopEdge;
	m_PixelY = Map(0, m_WindowHeight, m_CursorY, canvasTopEdge, 0 /* ? ? ? */);
	//}
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

	data = std::vector<GLubyte>(CanvasWidth * CanvasHeight * 4, 255);
}

void PaintAtPixelCoord()
{
	if (m_PixelX < 0 || m_PixelX >= CanvasWidth
		|| m_PixelY < 0 || m_PixelY >= CanvasHeight)
	{
		return;
	}

	data[m_PixelY * CanvasWidth * 4 + m_PixelX * 4] = 0; // R
	data[m_PixelY * CanvasWidth * 4 + m_PixelX * 4 + 1] = 0; // G
	data[m_PixelY * CanvasWidth * 4 + m_PixelX * 4 + 2] = 0; // B
	data[m_PixelY * CanvasWidth * 4 + m_PixelX * 4 + 3] = 255; // A
}