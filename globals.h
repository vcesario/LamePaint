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