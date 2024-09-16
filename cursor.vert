#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aCoord;

uniform vec2 windowSize;
uniform vec2 cursorPos;
uniform float brushSize;

out vec2 uvCoord;

void main()
{
	vec2 cursor_NdcPos = cursorPos / windowSize;
	cursor_NdcPos = (cursor_NdcPos * 2.0f) - vec2(1.0f, 1.0f);
	cursor_NdcPos.y *= -1.0f;

	vec2 cursor_NdcScale = vec2(brushSize, brushSize) / windowSize;

	gl_Position = vec4(aPos * vec3(cursor_NdcScale, 1.0f), 1.0f) + vec4(cursor_NdcPos, 0.0f, 0.0f);

	uvCoord = aCoord;
}