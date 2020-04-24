#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "Action.hpp"

class ActionManager final
{
public:
	static ActionManager& GetInstance()
	{
		// C++11 guarantees that this will be initialized in a thread-safe way.
		// Magic statics (function-local static initialization) are supported
		// in MSVC only since version 2015
		static ActionManager instance;
		return instance;
	}
	std::shared_ptr<Action> GetAction(const char* label)
	{
		auto it = actions.find(label);

		if (it == actions.end()) {
			return nullptr;
		}
		return std::make_shared<Action>(it->second);
	}

	std::shared_ptr<Action> AddAction(const char* label, ActionType type)
	{
		actions.insert(std::make_pair(label, Action(label, type)));
		auto it = actions.find(label);
		return std::make_shared<Action>(it->second);
	}

private:
	ActionManager() = default;
	~ActionManager() = default;

	// Delete the copy and move constructors
	ActionManager(const ActionManager&) = delete;
	ActionManager& operator=(const ActionManager&) = delete;
	ActionManager(ActionManager&&) = delete;
	ActionManager& operator=(ActionManager&&) = delete;

	std::unordered_map<const char*, Action> actions;
};