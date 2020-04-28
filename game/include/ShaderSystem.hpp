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
    }
}

void
ShaderSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{
    for (auto& entity : entities)
    {
        
    }
}
