#pragma once

#include "ecs.hpp"

class CameraSystem: public System
{
public:
    void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
    void Init();
};

void
CameraSystem::Init()
{

}

void
CameraSystem::Update(float                             dt,
                     std::shared_ptr<ComponentManager> componentManager)
{
    for (auto& entity : entities)
    {
        auto& camera = componentManager->GetComponent<Camera>(entity);
        auto& transform = componentManager->GetComponent<Transform>(entity);

        camera.cameraPos = transform.position;
    }
}
