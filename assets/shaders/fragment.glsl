#version 430 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform vec4 newColor;
uniform sampler2D texture1;

subroutine vec4 colorTemplate(vec4 tempColor);
subroutine uniform colorTemplate Color;

subroutine(colorTemplate)
vec4 ColorRed(vec4 tempColor)
{
	return tempColor * vec4(1.0, 0.0, 0.0, 1.0);
}

subroutine(colorTemplate)
vec4 ColorBlue(vec4 tempColor)
{
	return tempColor * vec4(0.0, 0.0, 1.0, 1.0);
}

subroutine(colorTemplate)
vec4 ColorWhite(vec4 tempColor)
{
	return tempColor * vec4(1.0, 1.0, 1.0, 1.0);
}

subroutine(colorTemplate)
vec4 ColorInverse(vec4 tempColor)
{
	return vec4(vec3(1.0 - tempColor.x, 1.0 - tempColor.y, 1.0 - tempColor.z), 1.0);
}

subroutine(colorTemplate)
vec4 ColorCustom(vec4 tempColor)
{
	float gray = dot(tempColor.rgb, vec3(0.3, 0.587, 0.1));
	gray = gray + 0.1;
	return vec4(vec3(gray), 1.0);
}

void main()
{
	vec4 tempColor = texture(texture1, TexCoord) * newColor;
	FragColor = Color(tempColor);
}