#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 Weights;
in vec4 BoneIDs;

//in vec4 FragPosLightSpace;

out vec4 FragColor;

struct DirLight
{
	vec3 direction;
	vec3 position; // Needed for shadows

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
uniform sampler2D shadowMap;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLight;

#define NR_SPOT_LIGHTS 32
uniform int numberOfSpotLights;
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

vec3 ResultDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 ResultPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 ResultSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir);

void main()
{

	/*
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	FragColor = texture(texture1, TexCoord);

	vec3 result = vec3(0.0, 0.0, 0.0);
	result += ResultPointLight(pointLight, norm, FragPos, viewDir);
	
	for (int i = 0; i < numberOfSpotLights; i++)
		result += ResultSpotLight(spotLights[i], norm, FragPos, viewDir);
	
	result += ResultDirLight(dirLight, norm, viewDir);
	*/
	vec4 weightsColor = Weights;
	FragColor = weightsColor;
	
}

vec3 ResultDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0f);
	//Blinn
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0f), 32);

	vec3 ambient = light.ambient * vec3(texture(texture1, TexCoord).rgb);
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture1, TexCoord).rgb);
	vec3 specular = light.specular * spec;
	
	vec3 result = (ambient + diffuse + specular);

	return result;
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
	float diff = max(dot(normal, lightDir), 0.0);
	//Blinn
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * vec3(texture(texture1, TexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture1, TexCoord));
	vec3 specular = light.specular * spec;

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 direction)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(-direction);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

