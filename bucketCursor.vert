#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aCoord;

uniform vec2 windowSize;
uniform vec2 cursorPos;

out vec2 uvCoord;

const vec2 brushSize = vec2(24.5, 24.5);
const vec2 bucketOffset = vec2(6, -6);

void main()
{
	vec2 cursor_NdcPos = (cursorPos + bucketOffset) / windowSize;
	cursor_NdcPos = (cursor_NdcPos * 2.0) - vec2(1.0, 1.0);
	cursor_NdcPos.y *= -1.0;

	vec2 cursor_NdcScale = brushSize / windowSize;
	gl_Position = vec4(aPos * vec3(cursor_NdcScale, 1.0), 1.0) + vec4(cursor_NdcPos, 0.0, 0.0);

	
	uvCoord = vec2(mix(brushSize.x * 1 / 98.0, brushSize.x * 2 / 98.0, aCoord.x),	// X
						mix(brushSize.y * 1 / 98.0, brushSize.y * 2 / 98.0, aCoord.y)); // Y
}