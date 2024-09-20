#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "globals.h"

void InitUI(GLFWwindow* window);
void DrawUI(int pixelX, int pixelY, float framerate, TextureObject texObj);
void TerminateUI();
void SetUIBrushSlider(int value);
bool IsCursorHoveringUI();