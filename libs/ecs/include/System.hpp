#pragma once

#include <memory>
#include <set>

#include "ECSTypes.hpp"
#include "ComponentManager.hpp"

class System
{
public:
	virtual void Update(float dt, std::shared_ptr<ComponentManager> componentManager) = 0;
	//virtual void Init(std::shared_ptr<ComponentManager> componentManager) = 0;
	std::set<Entity> entities;
};
