#version 430 core
out vec4 FragColor;

uniform vec4 boxColor;

void main()
{
    FragColor = boxColor;
}