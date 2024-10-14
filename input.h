#pragma once
#include <GLFW/glfw3.h>
#include <functional>

namespace LameInput
{
	extern double CursorX;
	extern double CursorY;
	extern double CursorX_LastFrame;
	extern double CursorY_LastFrame;
	extern bool IsClicking;
	extern bool IsDragging;
	extern bool ClickDownConsumed;

	void Init(GLFWwindow* window);
	int Subscribe_KeyChanged(std::function<void(int, int)> callback);
}