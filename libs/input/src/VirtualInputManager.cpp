#include "VirtualInputManager.hpp"

std::shared_ptr<VirtualInput> VirtualInputManager::AddVirtualInput(std::string label, VirtualInputType type) {
	inputs.insert(std::make_pair(label, std::make_shared<VirtualInput>(type)));
	return inputs[label];
}

std::shared_ptr<VirtualInput> VirtualInputManager::GetVirtualInput(std::string label) {
	if(inputs.find(label) == inputs.end()) {
		return nullptr;
	}
	else {
		return inputs[label];
	}
}

void VirtualInputManager::SetVirtualInputValue(std::string label, float newValue) {
	inputs[label]->UpdateRawValue(newValue);
}

void VirtualInputManager::Update(int rawInputId, int rawInputSource, float value) {
	std::pair<int, int> mappingsKey = std::make_pair(rawInputId, rawInputSource);
	std::string virtualInputLabel;

	if(mappings.find(mappingsKey) != mappings.end()) {
		virtualInputLabel = mappings[mappingsKey];
	} else {
		std::cout << "No assigned mapping found...\n";
		return;
	}

	if(inputs.find(virtualInputLabel) != inputs.end()) {
		inputs[virtualInputLabel]->UpdateRawValue(value);
	} else {
		std::cout << "[ERR] Assigned virtual input not found!\n";
		return;
	}
}

void VirtualInputManager::AddMapping(std::string virtualInputLabel, int rawInputId, int rawInputSource) {
	if(inputs.find(virtualInputLabel) != inputs.end()) {
		mappings.insert(std::make_pair(std::make_pair(rawInputId, rawInputSource), virtualInputLabel));
	}
	else {
		std::cout << "No virtual key found...\n";
	}
}

VirtualInputManager* VirtualInputManager::singleton_ = nullptr;