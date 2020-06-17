#version 430 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{   
    // https://en.wikipedia.org/wiki/List_of_refractive_indices - Plate Glass - 1.52
    // https://en.wikipedia.org/wiki/List_of_refractive_indices - Germanium   - 4.05
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}