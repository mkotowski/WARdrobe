#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
	vec3 direction;
	vec3 position;
	float cutOff;
    float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// texture samplers
uniform sampler2D texture1;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLight;

#define NR_SPOT_LIGHTS 32
uniform int numberOfSpotLights;
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

vec3 ResultDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 ResultPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 ResultSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	FragColor = texture(texture1, TexCoord);

	vec3 result = ResultDirLight(dirLight, norm, viewDir);
	result += ResultPointLight(pointLight, norm, FragPos, viewDir);
	
	for (int i = 0; i < numberOfSpotLights; i++)
		result += ResultSpotLight(spotLights[i], norm, FragPos, viewDir);
	
	FragColor = vec4(result, 1.0f);
}

vec3 ResultDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0f);
	//Blinn
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0f), 32);

	vec3 ambient = light.ambient * vec3(texture(texture1, TexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture1, TexCoord));
	vec3 specular = light.specular * spec;
	return (ambient + diffuse + specular);
}

vec3 ResultPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0f);
	//Blinn
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0f), 32);
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));


	vec3 ambient = light.ambient * vec3(texture(texture1, TexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture1, TexCoord));
	vec3 specular = light.specular * spec;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 ResultSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.direction - fragPos);
	float diff = max(dot(normal, lightDir), 0.0f);
	//Blinn
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0f), 32);
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

	vec3 ambient = light.ambient * vec3(texture(texture1, TexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture1, TexCoord));
	vec3 specular = light.specular * spec;

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}