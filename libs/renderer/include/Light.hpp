#pragma once

#include <string>
#include <glm/glm.hpp>

class Light 
{
public:
    Light();

    // Different constructors for each type of the light
    // Directional
    Light(glm::vec3, glm::vec3, glm::vec3 , glm::vec3, glm::vec3);
    // PointLight
    Light(glm::vec3 position, float constant, float linear, float quadratic, 
            glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor);
    // SpotLight
    Light(glm::vec3 direction, glm::vec3 position, float constant, float linear, 
            float quadratic, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor,
            float cutOff, float outerCutOff);

    // Different variables for each type of the lightning

    glm::vec3 direction;    // DirectionalLight, Spotlight
    glm::vec3 position;     // PointLight, Spotlight

    // PointLight, SpotLight
    float constant;
    float linear;
    float quadratic;

    // Spotlight
    float cutOff;
    float outerCutOff;

    // Colors
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;

    // Type of the lightning to set proper uniforms
    std::string typeOfTheLight;

    void InitFramebuffer();

};