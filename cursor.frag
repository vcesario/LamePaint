#version 330 core

in vec2 uvCoord;

uniform vec3 brushColor;

out vec4 FragColor;

void main()
{
	vec2 dxdy = uvCoord - 0.5;
	float d = pow(dxdy.x, 2.0) + pow(dxdy.y, 2.0);
	float inCircle = 1.0 - step(0.25, d);

	FragColor = vec4(brushColor / 255.0, 0.9) * inCircle;
}