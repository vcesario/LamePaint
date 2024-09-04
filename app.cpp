#include "app.h"
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <map>


struct vec3byte
{
	uint8_t x;
	uint8_t y;
	uint8_t z;

	vec3byte()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	vec3byte(uint8_t x, uint8_t y, uint8_t z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

int m_WindowWidth;
int m_WindowHeight;

int CanvasWidth;
int CanvasHeight;

std::vector<GLubyte> data;

const vec3byte COLOR_BLACK(0, 0, 0);
const vec3byte COLOR_WHITE(255, 255, 255);
const vec3byte COLOR_RED(255, 0, 0);
const vec3byte COLOR_GREEN(0, 255, 0);
const vec3byte COLOR_BLUE(0, 0, 255);
vec3byte m_BrushColor = COLOR_BLACK;
std::map<Colors, vec3byte> m_ColorIdToVal = {
	{Colors::Black, COLOR_BLACK},
	{Colors::White, COLOR_WHITE},
	{Colors::Red, COLOR_RED},
	{Colors::Green, COLOR_GREEN},
	{Colors::Blue, COLOR_BLUE},
};

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

void SetupCanvas(int windowWidth, int windowHeight)
{
	m_WindowWidth = windowWidth;
	m_WindowHeight = windowHeight;

	CanvasWidth = windowWidth;
	CanvasHeight = windowHeight - FrameHeight() * 2;

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
				data[j * CanvasWidth * 4 + i * 4] = m_BrushColor.x; // R
				data[j * CanvasWidth * 4 + i * 4 + 1] = m_BrushColor.y; // G
				data[j * CanvasWidth * 4 + i * 4 + 2] = m_BrushColor.z; // B
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

std::ostream& operator << (std::ostream& os, const vec3& s) {
	return (os << "(" << s.x << ", " << s.y << ", " << s.z << ")");
}

double MinOf4(double val1, double val2, double val3, double val4)
{
	double min = val1;

	if (val2 < min)
	{
		min = val2;
	}

	if (val3 < min)
	{
		min = val3;
	}

	if (val4 < min)
	{
		min = val4;
	}

	return min;
}
double MaxOf4(double val1, double val2, double val3, double val4)
{
	double max = val1;

	if (val2 > max)
	{
		max = val2;
	}

	if (val3 > max)
	{
		max = val3;
	}

	if (val4 > max)
	{
		max = val4;
	}

	return max;
}
double EdgeFunction(vec3 a, vec3 b, vec2 c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}
double Clamp(double value, double min, double max)
{
	if (value < min)
	{
		value = min;
	}
	if (value > max)
	{
		value = max;
	}
	return value;
}
void PaintRectangle(double cursorX_LastFrame, double cursorY_LastFrame, double cursorX, double cursorY, int brushSize)
{
	if (cursorX < 0 || cursorX >= CanvasWidth
		|| cursorY < FrameHeight() || cursorY >= (m_WindowHeight - FrameHeight()))
	{
		return;
	}

	//std::cout << std::fixed << std::setprecision(2) << "Drawing rectangle from (" << cursorX_LastFrame << ", " << cursorY_LastFrame << ") to ("
	//	<< cursorX << ", " << cursorY << ")" << std::endl;

	int pixelX_LastFrame = cursorX_LastFrame;
	int pixelY_LastFrame = cursorY_LastFrame - FrameHeight();
	int pixelX = cursorX;
	int pixelY = cursorY - FrameHeight();

	// find vector that is orthogonal to (C - Clf)
	// normalize it and multiply by half brushSize
	// the variation (dx,dy) from the resultant vector in relation to C is added and subtracted
	// from both C and Clf to find all four of the rectangles' vertices.
	// then, run the rasterization algorithm
	// C is Cursor
	// Clf is Cursor_LastFrame

	vec3 a(pixelX - pixelX_LastFrame, pixelY - pixelY_LastFrame, 0);
	vec3 b(0, 0, 1);

	//cross product = (a2 * b3 – a3 * b2) * i + (a3 * b1 – a1 * b3) * j + (a1 * b2 – a2 * b1) * k
	vec3 cross(a.y * b.z /* - a.z * b.y*/, /*a.z * b.x*/ -a.x * b.z, /*a.x * b.y - a.y * b.x*/ 0); // parts of formula commented out since b.x and b.y = 0
	//std::cout << std::fixed << std::setprecision(2) << "cross(" << cross.x << ", " << cross.y << ", " << cross.z << ")" << std::endl;
	double crossMag = sqrt(cross.x * cross.x + cross.y * cross.y);
	vec3 unit(cross.x / crossMag, cross.y / crossMag, 0);

	float halfBrush = (float)brushSize / 2.0f;
	vec3 halfBrushVec(unit.x * halfBrush, unit.y * halfBrush, 0);
	vec3 A(pixelX_LastFrame + halfBrushVec.x, pixelY_LastFrame + halfBrushVec.y, 0);
	vec3 B(pixelX + halfBrushVec.x, pixelY + halfBrushVec.y, 0);
	vec3 C(pixelX - halfBrushVec.x, pixelY - halfBrushVec.y, 0);
	vec3 D(pixelX_LastFrame - halfBrushVec.x, pixelY_LastFrame - halfBrushVec.y, 0);

	//std::cout /*<< std::fixed << std::setprecision(2)*/ << "v1: " << A << std::endl;
	//std::cout /*<< std::fixed << std::setprecision(2)*/ << "v2: " << B << std::endl;
	//std::cout /*<< std::fixed << std::setprecision(2)*/ << "v3: " << C << std::endl;
	//std::cout /*<< std::fixed << std::setprecision(2)*/ << "v4: " << D << std::endl;

	// rasterize quad
	double boxLeft = MinOf4(A.x, B.x, C.x, D.x);
	double boxRight = MaxOf4(A.x, B.x, C.x, D.x);
	double boxTop = MinOf4(A.y, B.y, C.y, D.y);
	double boxBottom = MaxOf4(A.y, B.y, C.y, D.y);

	boxLeft = Clamp(boxLeft, 0, CanvasWidth - 1);
	boxRight = Clamp(boxRight, 0, CanvasWidth - 1);
	boxTop = Clamp(boxTop, 0, CanvasHeight - 1);
	boxBottom = Clamp(boxBottom, 0, CanvasHeight- 1);

	for (int i = boxLeft; i < boxRight; i++)
	{
		for (int j = boxTop; j < boxBottom; j++)
		{
			vec2 P(i, j);
			double ABP = EdgeFunction(A, B, P);
			if (ABP >= 0)
			{
				double BCP = EdgeFunction(B, C, P);
				if (BCP >= 0)
				{
					double CDP = EdgeFunction(C, D, P);
					if (CDP >= 0)
					{
						double DAP = EdgeFunction(D, A, P);
						if (DAP >= 0)
						{
							// is inside quad, so draw pixel
							data[j * CanvasWidth * 4 + i * 4] = m_BrushColor.x; // R
							data[j * CanvasWidth * 4 + i * 4 + 1] = m_BrushColor.y; // G
							data[j * CanvasWidth * 4 + i * 4 + 2] = m_BrushColor.z; // B
							data[j * CanvasWidth * 4 + i * 4 + 3] = 255; // A
						}
					}
				}
			}
		}
	}
}

void SetBrushColor(Colors newColor)
{
	m_BrushColor = m_ColorIdToVal[newColor];
}