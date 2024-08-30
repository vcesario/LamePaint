#include <vector>
#include <glad/glad.h>

struct vec2
{
	double x;
	double y;

	vec2(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
};

extern int CanvasWidth;
extern int CanvasHeight;
extern std::vector<GLubyte> data;

vec2 GetCursorPos_Pixel(double cursorX, double cursorY);
void SetupCanvas(int width, int height);
void PaintAtPixelCoord(double cursorX, double cursorY, int brushSize);
