#pragma once

#include "ecs.hpp"
#include "ModelArray.hpp"
#include "Animator.hpp"

class AnimationSystem : public System
{
public:
    void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void Init(std::shared_ptr<ComponentManager> componentManager);
};


void AnimationSystem::Init(std::shared_ptr<ComponentManager> componentManager)
{
	for(auto& entity: entities)
	{
		auto& modelArray = componentManager->GetComponent<ModelArray>(entity);
		auto& animator = componentManager->GetComponent<Animator>(entity);
		animator.animatedModel = &modelArray.zeroLevelModel;
	}
    
}

void AnimationSystem::Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager)
{
	// for (auto& entity: entities)
	// {
	// 	auto& animator = componentManager->GetComponent<Animator>(entity);
	// 	animator.PlayCurrentAnimation(dt);
	// }
}