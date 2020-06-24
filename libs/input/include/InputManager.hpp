#pragma once

#include <memory>
#include <map>
#include <iostream>
#include <string>
#include <utility>

#include "VirtualInput.hpp"

class InputManager
{
public:
	InputManager() = default;
	~InputManager() = default;

	std::shared_ptr<VirtualInput> AddVirtualInput(std::string label, VirtualInputType type);
	std::shared_ptr<VirtualInput> GetVirtualInput(std::string label);
	void SetVirtualInputValue(std::string label, float newValue);
	void Update(int rawInputId, int rawInputSource, float value);
	void AddMapping(std::string virtualInputLabel, int rawInputId, int rawInputSource);
private:
	std::map<std::string,std::shared_ptr<VirtualInput>> inputs;

	// label raw, label virtual:
	std::map<std::pair<int, int>,std::string> mappings;
};