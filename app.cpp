#include "app.h"

#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <queue>
#include <unordered_set>

//#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define VecToIndex(Vec) Vec.y * CanvasWidth * 4 + Vec.x * 4
#define CoordToIndex(X, Y) Y * CanvasWidth * 4 + X * 4

namespace App
{
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
	const vec3byte COLOR_YELLOW(240, 200, 0);

	std::map<Colors, vec3byte> m_ColorIdToVal = {
		{Colors::Black, COLOR_BLACK},
		{Colors::White, COLOR_WHITE},
		{Colors::Red, COLOR_RED},
		{Colors::Green, COLOR_GREEN},
		{Colors::Blue, COLOR_BLUE},
		{Colors::Yellow, COLOR_YELLOW},
	};

	vec3byte m_BrushColor = COLOR_BLACK;
	vec3byte m_EraserColor = COLOR_WHITE;

	int m_BrushSize = 5;
	int m_EraserSize = 3;

	BrushModes m_CurrentMode = BrushModes::DEFAULT;

	void Init()
	{

	}

	void ProcessInput()
	{

	}

	void Render()
	{

	}

	void Terminate()
	{

	}

	bool IsPointWithinCircle(int pointX, int pointY, int centerX, int centerY, int r)
	{
		int dx = pointX - centerX;
		int dy = pointY - centerY;
		int d = dx * dx + dy * dy;
		return d <= r * r;
	}

	bool IsCursorWithinCanvas(double x, double y)
	{
		if (x < 0 || x >= CanvasWidth || y < FrameHeight() || y >= (m_WindowHeight - FrameHeight()))
		{
			return false;
		}

		return true;
	}
	bool IsCanvasCoordWithinCanvas(double x, double y)
	{
		if (x < 0 || x >= CanvasWidth || y < 0 || y >= CanvasHeight)
		{
			return false;
		}

		return true;
	}

	int FrameHeight()
	{
		return 19;
	}

	vec2int CursorToCanvas(double cursorX, double cursorY)
	{
		int pixelX = cursorX;
		int pixelY = cursorY - FrameHeight();
		return vec2int(pixelX, pixelY);
	}

	void SetupCanvas(int windowWidth, int windowHeight)
	{
		m_WindowWidth = windowWidth;
		m_WindowHeight = windowHeight;

		CanvasWidth = windowWidth;
		CanvasHeight = windowHeight - FrameHeight() * 2;

		data = std::vector<GLubyte>(CanvasWidth * CanvasHeight * 4, 255);
	}

	void PaintCircle(double cursorX, double cursorY)
	{
		if (m_CurrentMode == BrushModes::BUCKET)
		{
			return;
		}

		if (!IsCursorWithinCanvas(cursorX, cursorY))
		{
			return;
		}

		int brushSize = m_CurrentMode == BrushModes::DEFAULT ? m_BrushSize : m_EraserSize;

		vec2int canvasCoord = CursorToCanvas(cursorX, cursorY);
		int pixelX = canvasCoord.x;
		int pixelY = canvasCoord.y;

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
					if (m_CurrentMode == BrushModes::DEFAULT)
					{
						data[CoordToIndex(i, j)] = m_BrushColor.x; // R
						data[CoordToIndex(i, j) + 1] = m_BrushColor.y; // G
						data[CoordToIndex(i, j) + 2] = m_BrushColor.z; // B
					}
					else // mode = eraser
					{
						data[CoordToIndex(i, j)] = m_EraserColor.x; // R
						data[CoordToIndex(i, j) + 1] = m_EraserColor.y; // G
						data[CoordToIndex(i, j) + 2] = m_EraserColor.z; // B
					}

					data[CoordToIndex(i, j) + 3] = 255; // A
				}
			}
		}
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
	void PaintRectangle(double cursorX_LastFrame, double cursorY_LastFrame, double cursorX, double cursorY)
	{
		if (m_CurrentMode == BrushModes::BUCKET)
		{
			return;
		}

		if (!IsCursorWithinCanvas(cursorX, cursorY))
		{
			return;
		}

		int brushSize = m_CurrentMode == BrushModes::DEFAULT ? m_BrushSize : m_EraserSize;


		//std::cout << std::fixed << std::setprecision(2) << "Drawing rectangle from (" << cursorX_LastFrame << ", " << cursorY_LastFrame << ") to ("
		//	<< cursorX << ", " << cursorY << ")" << std::endl;


		vec2int canvasCoord_LastFrame = CursorToCanvas(cursorX_LastFrame, cursorY_LastFrame);
		int pixelX_LastFrame = canvasCoord_LastFrame.x;
		int pixelY_LastFrame = canvasCoord_LastFrame.y;

		vec2int canvasCoord = CursorToCanvas(cursorX, cursorY);
		int pixelX = canvasCoord.x;
		int pixelY = canvasCoord.y;

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
		boxBottom = Clamp(boxBottom, 0, CanvasHeight - 1);

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
								if (m_CurrentMode == BrushModes::DEFAULT)
								{
									data[CoordToIndex(i, j)] = m_BrushColor.x; // R
									data[CoordToIndex(i, j) + 1] = m_BrushColor.y; // G
									data[CoordToIndex(i, j) + 2] = m_BrushColor.z; // B
								}
								else // mode = eraser
								{
									data[CoordToIndex(i, j)] = m_EraserColor.x; // R
									data[CoordToIndex(i, j) + 1] = m_EraserColor.y; // G
									data[CoordToIndex(i, j) + 2] = m_EraserColor.z; // B
								}

								data[CoordToIndex(i, j) + 3] = 255; // A
							}
						}
					}
				}
			}
		}
	}

	bool IsSameColor(vec2int c1, vec2int c2)
	{
		//std::cout << c1.x << ", " << c1.y << " x " << c2.x << ", " << c2.y << std::endl;
		//std::cout << (int)data[VecToIndex(c1)] << " == " << (int)data[VecToIndex(c2)] << ": " << (data[VecToIndex(c1)] == data[VecToIndex(c2)]) << std::endl;
		//std::cout << (int)data[VecToIndex(c1) + 1] << " == " << (int)data[VecToIndex(c2) + 1] << ": " << (data[VecToIndex(c1) + 1] == data[VecToIndex(c2) + 1]) << std::endl;
		//std::cout << (int)data[VecToIndex(c1) + 2] << " == " << (int)data[VecToIndex(c2) + 2] << ": " << (data[VecToIndex(c1) + 2] == data[VecToIndex(c2) + 2]) << std::endl;

		return data[VecToIndex(c1)] == data[VecToIndex(c2)] // is R the same?
			&& data[VecToIndex(c1) + 1] == data[VecToIndex(c2) + 1] // is G the same?
			&& data[VecToIndex(c1) + 2] == data[VecToIndex(c2) + 2]; // is B the same?
	}
	void PaintFill(double cursorX, double cursorY)
	{
		if (m_CurrentMode != BrushModes::BUCKET)
		{
			return;
		}

		if (!IsCursorWithinCanvas(cursorX, cursorY))
		{
			return;
		}

		vec2int canvasCoord = CursorToCanvas(cursorX, cursorY);

		// if pixel at point is exactly the same color, no need to go further
		if (data[VecToIndex(canvasCoord)] == m_BrushColor.x
			&& data[VecToIndex(canvasCoord) + 1] == m_BrushColor.y
			&& data[VecToIndex(canvasCoord) + 2] == m_BrushColor.z)
		{
			std::cout << "pixel is already that color!" << std::endl;
			return;
		}

		// flood fill algorithm
		std::queue<vec2int> coords; // coords to be painted possibly
		std::unordered_set<vec2int, vec2intHash> visited = { canvasCoord };
		coords.push(canvasCoord);

		while (coords.size() > 0)
		{
			// grabs coord at the top of the q
			vec2int coord = coords.front();
			coords.pop();

			// adds cardinal neighbors to end of q if they're the same color as coord (and haven't been visited yet)
			vec2int coordAbove(coord.x, coord.y - 1);
			if (IsCanvasCoordWithinCanvas(coordAbove.x, coordAbove.y) && !visited.count(coordAbove) && IsSameColor(coord, coordAbove))
			{
				coords.push(coordAbove);
				visited.insert(coordAbove);
			}

			vec2int coordLeft(coord.x - 1, coord.y);
			if (IsCanvasCoordWithinCanvas(coordLeft.x, coordLeft.y) && !visited.count(coordLeft) && IsSameColor(coord, coordLeft))
			{
				coords.push(coordLeft);
				visited.insert(coordLeft);
			}

			vec2int coordBelow(coord.x, coord.y + 1);
			if (IsCanvasCoordWithinCanvas(coordBelow.x, coordBelow.y) && !visited.count(coordBelow) && IsSameColor(coord, coordBelow))
			{
				coords.push(coordBelow);
				visited.insert(coordBelow);
			}

			vec2int coordRight(coord.x + 1, coord.y);
			if (IsCanvasCoordWithinCanvas(coordRight.x, coordRight.y) && !visited.count(coordRight) && IsSameColor(coord, coordRight))
			{
				coords.push(coordRight);
				visited.insert(coordRight);
			}

			// sets pixel at coord to brush color
			data[VecToIndex(coord)] = m_BrushColor.x;
			data[VecToIndex(coord) + 1] = m_BrushColor.y;
			data[VecToIndex(coord) + 2] = m_BrushColor.z;
		}
	}

	void SetBrushColor(Colors newColor)
	{
		switch (m_CurrentMode)
		{
		case BrushModes::DEFAULT:
		case BrushModes::BUCKET:
			m_BrushColor = m_ColorIdToVal[newColor];
			break;
		case BrushModes::ERASER:
			m_EraserColor = m_ColorIdToVal[newColor];
			break;
		}
	}
	vec3byte GetBrushColor()
	{
		switch (m_CurrentMode)
		{
		case BrushModes::DEFAULT:
		case BrushModes::BUCKET:
			return m_BrushColor;
		case BrushModes::ERASER:
			return m_EraserColor;
		}
	}

	void SetModeToDefault()
	{
		m_CurrentMode = BrushModes::DEFAULT;
	}

	void SetModeToEraser()
	{
		m_CurrentMode = BrushModes::ERASER;
	}

	void SetModeToBucket()
	{
		m_CurrentMode = BrushModes::BUCKET;
	}

	void SwapBrushMode()
	{
		switch (m_CurrentMode)
		{
		case BrushModes::DEFAULT:
			m_CurrentMode = BrushModes::ERASER;
			break;
		case BrushModes::ERASER:
			m_CurrentMode = BrushModes::DEFAULT;
			break;
		default:
			break;
		}
	}

	BrushModes GetBrushMode()
	{
		return m_CurrentMode;
	}

	void SetBrushSize(int size)
	{
		switch (m_CurrentMode)
		{
		case BrushModes::DEFAULT:
			m_BrushSize = size;
			break;
		case BrushModes::ERASER:
			m_EraserSize = size;
			break;
		default:
			break;
		}
	}

	int GetBrushSize()
	{
		switch (m_CurrentMode)
		{
		case BrushModes::DEFAULT:
			return m_BrushSize;
		case BrushModes::ERASER:
			return m_EraserSize;
		default:
			return 0;
		}
	}

	void ClearCanvas()
	{
		data = std::vector<GLubyte>(CanvasWidth * CanvasHeight * 4, 255);

		BindCanvasTexture();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CanvasWidth, CanvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	}

	void TransferTexToCanvas(TextureObject tex)
	{
		// declare pixels[proper size]
		GLubyte* pixels = new GLubyte[tex.width * tex.height * 4];
		// Bind texture
		glBindTexture(GL_TEXTURE_2D, tex.id);
		// getTexture to pixels
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		// for loop transfering
		for (size_t i = 0; i < tex.width && i < CanvasWidth; i++)
		{
			for (size_t j = 0; j < tex.height && j < CanvasHeight; j++)
			{
				data[CoordToIndex(i, j)] = pixels[j * tex.width * 4 + i * 4];
				data[CoordToIndex(i, j) + 1] = pixels[j * tex.width * 4 + i * 4 + 1];
				data[CoordToIndex(i, j) + 2] = pixels[j * tex.width * 4 + i * 4 + 2];
				data[CoordToIndex(i, j) + 3] = pixels[j * tex.width * 4 + i * 4 + 3];
			}
		}

		// update canvas texture
		BindCanvasTexture();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CanvasWidth, CanvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	}

	void SaveTexToDisk(std::string filePath)
	{
		if (!stbi_write_png(filePath.c_str(), CanvasWidth, CanvasHeight, 4, &data[0], CanvasWidth * 4))
		{
			std::cout << "ENCOUNTERED AN ERROR WHILE WRITING PNG." << std::endl;
		}
	}
}
