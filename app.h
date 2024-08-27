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

void UpdateCursorPos(double xpos, double ypos);
vec2 GetCursorPos_Pixel();
void SetupCanvas(int width, int height);