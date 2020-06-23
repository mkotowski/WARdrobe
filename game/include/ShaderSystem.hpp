#pragma once

#include "ecs.hpp"
#include "Shader.hpp"
#include <map>

class ShaderSystem : public System
{
    public:
    void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void Init(std::shared_ptr<ComponentManager> componentManager);
    std::map<std::string, Entity> shaders;
    Entity cameraEntity;
    float dtForSinX = 0.0f;

};

void
ShaderSystem::Init(std::shared_ptr<ComponentManager> componentManager)
{
    for (auto& entity: entities)
    {
        shaders.insert({
                        componentManager->GetComponent<Shader>(entity).shaderType, 
                        entity
                        });
        auto& shader = componentManager->GetComponent<Shader>(entity);
        shader.currentSubroutine = "ColorWhite";
        std::cout << shader.currentSubroutine << std::endl;
    }
}

void
ShaderSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{
    dtForSinX += dt;
    if (dtForSinX >= 3.14f)
        dtForSinX = 0.0f;
    
    for (auto& entity: entities)
    {
        auto& shader = componentManager->GetComponent<Shader>(entity);
        shader.use();
        shader.setVec3("viewPos", componentManager->GetComponent<Camera>(this->cameraEntity).cameraPos);       
        if (shader.shaderType == "modelShader" || shader.shaderType == "animatedModelShader" || shader.shaderType == "quadShader")
            shader.setFloat("sinX", sin((double)dtForSinX));     
    }
}
