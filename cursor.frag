#version 330 core

in vec2 uvCoord;

uniform vec3 brushColor;

out vec4 FragColor;

void main()
{
	vec2 dxdy = uvCoord - 0.5f;
	float d = pow(dxdy.x, 2.0f) + pow(dxdy.y, 2.0f);
	float inCircle = 1.0f - step(0.25f, d);

	FragColor = vec4(brushColor / 255.0f, 0.9f) * inCircle;
}