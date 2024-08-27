#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;
  
void main()
{
    //FragColor = vec4(TexCoord, 0.0, 1.0);
    FragColor = vec4(texture(tex, TexCoord).rgb, 1.0);
} 