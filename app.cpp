#include "app.h"
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include <vector>

int m_WindowWidth;
int m_WindowHeight;

int CanvasWidth;
int CanvasHeight;

std::vector<GLubyte> data;

bool IsPointWithinCircle(int pointX, int pointY, int centerX, int centerY, int r);

int FrameHeight()
{
	return 19;
}

vec2 GetCursorPos_Pixel(double cursorX, double cursorY)
{
	//return vec2(m_CursorX, m_CursorY);
	int pixelX = cursorX;
	int pixelY = cursorY - FrameHeight();
	return vec2(pixelX, pixelY);
}

void SetupCanvas(int width, int height)
{
	m_WindowWidth = width;
	m_WindowHeight = height;

	CanvasWidth = width;
	CanvasHeight = height - FrameHeight() * 2;

	data = std::vector<GLubyte>(CanvasWidth * CanvasHeight * 4, 255);
}

void PaintAtPixelCoord(double cursorX, double cursorY, int brushSize)
{
	if (cursorX < 0 || cursorX >= CanvasWidth
		|| cursorY < FrameHeight() || cursorY >= (m_WindowHeight - FrameHeight()))
	{
		return;
	}

	int pixelX = cursorX;
	int pixelY = cursorY - FrameHeight();

	unsigned int halfSize = brushSize / 2;
	bool isPair = brushSize % 2 == 0;
	int topLeftX = isPair ? pixelX - halfSize + 1 : pixelX - halfSize;
	int topLeftY = isPair ? pixelY - halfSize + 1 : pixelY - halfSize;
	int bottomRightX = pixelX + halfSize;
	int bottomRightY = pixelY + halfSize;

	if (topLeftX < 0)
	{
		topLeftX = 0;
	}
	if (topLeftY < 0)
	{
		topLeftY = 0;
	}
	if (bottomRightX >= CanvasWidth)
	{
		bottomRightX = CanvasWidth - 1;
	}
	if (bottomRightY >= CanvasHeight)
	{
		bottomRightY = CanvasHeight - 1;
	}

	for (int i = topLeftX; i <= bottomRightX; i++)
	{
		for (int j = topLeftY; j <= bottomRightY; j++)
		{
			if (IsPointWithinCircle(i, j, pixelX, pixelY, halfSize))
			{
				data[j * CanvasWidth * 4 + i * 4] = 0; // R
				data[j * CanvasWidth * 4 + i * 4 + 1] = 0; // G
				data[j * CanvasWidth * 4 + i * 4 + 2] = 0; // B
				data[j * CanvasWidth * 4 + i * 4 + 3] = 255; // A
			}
		}
	}
}

bool IsPointWithinCircle(int pointX, int pointY, int centerX, int centerY, int r)
{
	int dx = pointX - centerX;
	int dy = pointY - centerY;
	int d = dx * dx + dy * dy;
	return d <= r * r;
}