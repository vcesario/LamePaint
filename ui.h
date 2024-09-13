#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void InitUI(GLFWwindow* window);
void DrawUI(int pixelX, int pixelY, float framerate);
void TerminateUI();

void SetUIBrushSlider(int value);
