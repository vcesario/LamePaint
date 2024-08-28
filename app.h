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

void UpdateCursorPos(double xpos, double ypos);
vec2 GetCursorPos_Pixel();
void SetupCanvas(int width, int height);
void PaintAtPixelCoord();