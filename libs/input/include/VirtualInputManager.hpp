#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "VirtualInput.hpp"

class VirtualInputManager
{
public:
	~VirtualInputManager() = default;

	std::shared_ptr<VirtualInput> AddVirtualInput(std::string      label,
	                                              VirtualInputType type);
	std::shared_ptr<VirtualInput> GetVirtualInput(std::string label);
	void SetVirtualInputValue(std::string label, float newValue);
	void Update(int rawInputId, int rawInputSource, float value);
	void AddMapping(std::string virtualInputLabel,
	                int         rawInputId,
	                int         rawInputSource);
	static VirtualInputManager* GetInstance()
	{
		if (singleton_ == nullptr) {
			singleton_ = new VirtualInputManager();
		}
		return singleton_;
	}

	std::string GetAssignedLabel(int rawInputId, int rawInputSource)
	{
		std::pair<int, int> key = std::make_pair(rawInputId, rawInputSource);

		if (mappings.find(key) ==
		    mappings.end()) {
			return std::string("");
		} else {
			return mappings[key];
		}
	}

private:
	VirtualInputManager() = default;

	std::map<std::string, std::shared_ptr<VirtualInput>> inputs;

	// label raw, label virtual:
	std::map<std::pair<int, int>, std::string> mappings;

	static VirtualInputManager* singleton_;
};
