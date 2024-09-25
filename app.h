#include <vector>
#include <glad/glad.h>
#include <map>
#include "globals.h"

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

struct vec2int
{
	int x;
	int y;

	vec2int(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	bool operator==(const vec2int& other) const
	{
		return x == other.x && y == other.y;
	}
};
struct vec2intHash {
public:
	size_t operator()(const vec2int p) const
	{
		return std::hash<uint32_t>()(p.x) ^ std::hash<uint32_t>()(p.y);
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

struct vec3byte
{
	uint8_t x;
	uint8_t y;
	uint8_t z;

	vec3byte()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	vec3byte(uint8_t x, uint8_t y, uint8_t z)
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
	Yellow,
};

extern int CanvasWidth;
extern int CanvasHeight;
extern std::vector<GLubyte> data;
extern std::map<Colors, vec3byte> m_ColorIdToVal;

int FrameHeight();
vec2int CursorToCanvas(double cursorX, double cursorY);
void SetupCanvas(int windowWidth, int windowHeight);
void PaintCircle(double cursorX, double cursorY);
void PaintRectangle(double cursorX_LastFrame, double cursorY_LastFrame, double cursorX, double cursorY);
void PaintFill(double cursorX, double cursorY);
void SetBrushColor(Colors newColor);
vec3byte GetBrushColor();
void SetModeToDefault();
void SetModeToEraser();
void SetModeToBucket();
void SwapBrushMode();
BrushModes GetBrushMode();
void SetBrushSize(int size);
int GetBrushSize();

void ClearCanvas();
void TransferTexToCanvas(TextureObject tex);