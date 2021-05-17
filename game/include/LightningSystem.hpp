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
	void Init(std::shared_ptr<ComponentManager> componentManager);
    std::map<std::string, Entity> *shaders;
    Entity dirLight;
};

void
LightningSystem::Init(std::shared_ptr<ComponentManager> componentManager)
{
    for (auto& entity : entities)
    {
        auto& light = componentManager->GetComponent<Light>(entity);
        if (light.typeOfTheLight == "directionalLight")
        {
            dirLight = entity;
            return;
        }
            

    }
}


void
LightningSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{
    int spotLightCounter = 0;
    int pointLightCounter = 0;
    // Set all uniforms
    for(auto& entity: entities)
    {
        auto& light = componentManager->GetComponent<Light>(entity);
        if (light.typeOfTheLight == "directionalLight")
        {
            for(std::map<std::string, Entity>::iterator it = shaders->begin(); it != shaders->end(); it++)
            {
                auto& value = componentManager->GetComponent<Shader>(it->second);
                if (value.shaderType == "modelShader" || value.shaderType == "animatedModelShader")
                {
                    value.use();
                    value.setVec3("dirLight.direction",
                             light.direction);
                    value.setVec3("dirLight.ambient",
                                light.ambientColor);
                    value.setVec3("dirLight.diffuse",
                                light.diffuseColor);
                    value.setVec3("dirLight.specular",
                                light.specularColor);
                }
               
            }
            
        }
        else if (componentManager->GetComponent<Light>(entity).typeOfTheLight == "pointLight")
        {

            std::string pointLightName = "pointLights[" + std::to_string(pointLightCounter) + "].";

            for(std::map<std::string, Entity>::iterator it = shaders->begin(); it != shaders->end(); it++)
            {
                auto& value = componentManager->GetComponent<Shader>(it->second);
                if (value.shaderType == "modelShader" || value.shaderType == "animatedModelShader")
                {
                    value.use();
                    value.setVec3(pointLightName + "position",
                            componentManager->GetComponent<Light>(entity).position);
                    value.setFloat(pointLightName + "constant",
                                componentManager->GetComponent<Light>(entity).constant);
                    value.setFloat(pointLightName + "linear",
                                componentManager->GetComponent<Light>(entity).linear);
                    value.setFloat(pointLightName + "quadratic",
                                componentManager->GetComponent<Light>(entity).quadratic);
                    value.setVec3(pointLightName + "ambient",
                                componentManager->GetComponent<Light>(entity).ambientColor);
                    value.setVec3(pointLightName + "diffuse",
                                componentManager->GetComponent<Light>(entity).diffuseColor);
                    value.setVec3(pointLightName + "specular",
                                componentManager->GetComponent<Light>(entity).specularColor);
                }
                
            }
            pointLightCounter++;
        }
        else if (componentManager->GetComponent<Light>(entity).typeOfTheLight == "spotLight")
        {
            
            std::string spotLightName = "spotLights[" + std::to_string(spotLightCounter) + "].";
            
            for(std::map<std::string, Entity>::iterator it = shaders->begin(); it != shaders->end(); it++)
            {
                auto& value = componentManager->GetComponent<Shader>(it->second);
                if (value.shaderType == "modelShader" || value.shaderType == "animatedModelShader")
                {
                    value.use();
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
        value.use();
        value.setInt("numberOfSpotLights", spotLightCounter);
        value.setInt("numberOfPointLights", pointLightCounter);
    }
}