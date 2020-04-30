#pragma once

#include "ecs.hpp"
#include "Light.hpp"
#include "Shader.hpp"
#include <map>


class LightningSystem : public System
{
public:
    void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void Init();
    std::map<std::string, Entity> *shaders;
};

void
LightningSystem::Init()
{
    
}

void
LightningSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{
    int spotLightCounter = 0;
    // Set all uniforms
    for(auto& entity: entities)
    {
        if (componentManager->GetComponent<Light>(entity).typeOfTheLight == "directionalLight")
        {
            for(std::map<std::string, Entity>::iterator it = shaders->begin(); it != shaders->end(); it++)
            {
                auto& value = componentManager->GetComponent<Shader>(it->second);
                if (value.shaderType == "modelShader")
                {
                     value.setVec3("dirLight.direction",
                             componentManager->GetComponent<Light>(entity).direction);
                    value.setVec3("dirLight.ambient",
                                componentManager->GetComponent<Light>(entity).ambientColor);
                    value.setVec3("dirLight.diffuse",
                                componentManager->GetComponent<Light>(entity).diffuseColor);
                    value.setVec3("dirLight.specular",
                                componentManager->GetComponent<Light>(entity).specularColor);
                }
               
            }
        }
        else if (componentManager->GetComponent<Light>(entity).typeOfTheLight == "pointLight")
        {
            for(std::map<std::string, Entity>::iterator it = shaders->begin(); it != shaders->end(); it++)
            {
                auto& value = componentManager->GetComponent<Shader>(it->second);
                if (value.shaderType == "modelShader")
                {
                    value.setVec3("pointLight.position",
                            componentManager->GetComponent<Light>(entity).position);
                    value.setFloat("pointLight.constant",
                                componentManager->GetComponent<Light>(entity).constant);
                    value.setFloat("pointLight.linear",
                                componentManager->GetComponent<Light>(entity).linear);
                    value.setFloat("pointLight.quadratic",
                                componentManager->GetComponent<Light>(entity).quadratic);
                    value.setVec3("pointLight.ambient",
                                componentManager->GetComponent<Light>(entity).ambientColor);
                    value.setVec3("pointLight.diffuse",
                                componentManager->GetComponent<Light>(entity).diffuseColor);
                    value.setVec3("pointLight.specular",
                                componentManager->GetComponent<Light>(entity).specularColor);
                }
                
            }
        }
        else if (componentManager->GetComponent<Light>(entity).typeOfTheLight == "spotLight")
        {
            
            std::string spotLightName = "spotLights[" + std::to_string(spotLightCounter) + "].";
            
            for(std::map<std::string, Entity>::iterator it = shaders->begin(); it != shaders->end(); it++)
            {
                auto& value = componentManager->GetComponent<Shader>(it->second);
                if (value.shaderType == "modelShader")
                {
                     value.setVec3(spotLightName + "direction",
                            componentManager->GetComponent<Light>(entity).direction);
                    value.setVec3(spotLightName + "position",
                                componentManager->GetComponent<Light>(entity).position);
                    value.setFloat(spotLightName + "constant",
                                componentManager->GetComponent<Light>(entity).constant);
                    value.setFloat(spotLightName+ "linear",
                                componentManager->GetComponent<Light>(entity).linear);
                    value.setFloat(spotLightName + "quadratic",
                                componentManager->GetComponent<Light>(entity).quadratic);
                    value.setFloat(spotLightName + "cutOff",
                                glm::cos(glm::radians(componentManager->GetComponent<Light>(entity).cutOff)));
                    value.setFloat(spotLightName + "outerCutOff",
                                glm::cos(glm::radians(componentManager->GetComponent<Light>(entity).outerCutOff)));
                    value.setVec3(spotLightName + "ambient",
                                componentManager->GetComponent<Light>(entity).ambientColor);
                    value.setVec3(spotLightName + "diffuse",
                                componentManager->GetComponent<Light>(entity).diffuseColor);
                    value.setVec3(spotLightName + "specular",
                                componentManager->GetComponent<Light>(entity).specularColor);
                }
               
            }
            spotLightCounter++;
        }
    }
    // Set size of the spot lights
    for(std::map<std::string, Entity>::iterator it = shaders->begin(); it != shaders->end(); it++)
    {
        auto& value = componentManager->GetComponent<Shader>(it->second);
        value.setInt("numberOfSpotLights", spotLightCounter);
    }
}