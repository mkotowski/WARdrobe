#include "Light.hpp"

// default contructor
Light::Light()
{

}


// Directional Light
Light::Light(glm::vec3 direction, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor)
{
    this->direction = direction;
    this->ambientColor = ambientColor;
    this->diffuseColor = diffuseColor;
    this->specularColor = specularColor;
    this->typeOfTheLight = "directionalLight";
}

//Point Light
Light::Light(glm::vec3 position, float constant, float linear, float quadratic, 
            glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor)
{
    this->position = position;
    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;

    this->ambientColor = ambientColor;
    this->diffuseColor = diffuseColor;
    this->specularColor = specularColor;
    this->typeOfTheLight = "pointLight";
}

// Spot Light
Light::Light(glm::vec3 direction, glm::vec3 position, float constant, float linear, 
            float quadratic, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor,
            float cutOff, float outerCutOff)
{
    this->direction = direction;
    this->position = position;
    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;

    this->cutOff = cutOff;
    this->outerCutOff = outerCutOff;

    this->ambientColor = ambientColor;
    this->diffuseColor = diffuseColor;
    this->specularColor = specularColor;
    this->typeOfTheLight = "spotLight";
}