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

struct vec3
{
	double x;
	double y;
	double z;

	vec3(double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

enum class Colors
{
	Black,
	White,
	Red,
	Green,
	Blue,
};

extern int CanvasWidth;
extern int CanvasHeight;
extern std::vector<GLubyte> data;

int FrameHeight();
vec2 GetCursorPos_Pixel(double cursorX, double cursorY);
void SetupCanvas(int windowWidth, int windowHeight);
void PaintAtPixelCoord(double cursorX, double cursorY, int brushSize);
void PaintRectangle(double cursorX_LastFrame, double cursorY_LastFrame, double cursorX, double cursorY, int brushSize);
void SetBrushColor(Colors newColor);
