#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "globals.h"

namespace LameUI
{
	void Init(GLFWwindow* window);
	void Render(GLFWwindow* window, int pixelX, int pixelY, float framerate, TextureObject texObj);
	void Terminate();
	void SetBrushSlider(int value);
}