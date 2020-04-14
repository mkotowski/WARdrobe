#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>
#include <vector>

#include "ComponentManager.hpp"
#include "ECSTypes.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"

class GameplayManager
{
public:
	void Init()
	{
		// Create pointers to each manager
		componentManager = std::make_shared<ComponentManager>();
		entityManager = std::make_shared<EntityManager>();
		systemManager = std::make_shared<SystemManager>();
	}

	// Entity methods
	Entity CreateEntity() { return entityManager->CreateEntity(); }

	void DestroyEntity(Entity entity)
	{
		entityManager->DestroyEntity(entity);
		componentManager->EntityDestroyed(entity);
		systemManager->EntityDestroyed(entity);
	}

	// Component methods
	template<typename T>
	void RegisterComponent()
	{
		componentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		componentManager->AddComponent<T>(entity, component);

		auto signature = entityManager->GetSignature(entity);
		signature.set(componentManager->GetComponentType<T>(), true);
		entityManager->SetSignature(entity, signature);

		systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		componentManager->RemoveComponent<T>(entity);

		auto signature = entityManager->GetSignature(entity);
		signature.set(componentManager->GetComponentType<T>(), false);
		entityManager->SetSignature(entity, signature);

		systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		return componentManager->GetComponent<T>(entity);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return componentManager->GetComponentType<T>();
	}

	// System methods
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		return systemManager->RegisterSystem<T>();
	}

	template<typename T>
	void SetSystemSignature(Signature signature)
	{
		systemManager->SetSignature<T>(signature);
	}

	template<typename T>
	void SetRequiredComponent(ComponentType componentType)
	{
		systemManager->SetRequiredComponent<T>(componentType);
	}

	void Update(float dt) { systemManager->Update(dt, componentManager); }

	std::shared_ptr<ComponentManager> GetComponentManager()
	{
		return componentManager;
	}
	std::shared_ptr<EntityManager> GetEntityManager() { return entityManager; }
	std::shared_ptr<SystemManager> GetSystemManager() { return systemManager; }

private:
	std::shared_ptr<ComponentManager> componentManager;
	std::shared_ptr<EntityManager>    entityManager;
	std::shared_ptr<SystemManager>    systemManager;
};
