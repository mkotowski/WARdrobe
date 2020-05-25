#version 430 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform vec4 newColor;
uniform sampler2D texture1;


void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = texture(texture1, TexCoord) * newColor;
}