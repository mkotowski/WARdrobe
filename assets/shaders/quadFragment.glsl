#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float brightness;
uniform float contrast;

// sin(x), where x [0, 3.14]
uniform float sinX;
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
vec4 ColorWavy(vec4 tempColor)
{
	return tempColor * vec4(0.1, clamp(sinX, 0.3, 1.0), 0.1, 1.0);
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
    vec3 col = texture(screenTexture, TexCoords).rgb;
    vec4 result = Color(vec4(col, 1.0));
	vec3 colorContrasted = result.rgb * contrast;
	vec3 colorBright = colorContrasted + vec3(brightness, brightness, brightness);
    FragColor = vec4(colorBright, 1.0);
}