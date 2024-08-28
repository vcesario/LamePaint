#ifndef APP_H
#define APP_H

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

void UpdateCursorPos(double xpos, double ypos);
vec2 GetCursorPos_Pixel();
void SetupCanvas(int width, int height);
#endif