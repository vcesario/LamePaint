#pragma once
#include <glad/glad.h>

struct TextureObject
{
	GLuint id;
	int width;
	int height;

	TextureObject(GLuint id, int width, int height)
	{
		this->id = id;
		this->width = width;
		this->height = height;
	}
};

enum class BrushModes
{
	DEFAULT,
	ERASER,
	BUCKET,
};

bool LoadTextureFromFile(const char* file_name, GLuint* out_texture, int* out_width, int* out_height);