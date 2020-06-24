#pragma once

#include <memory>
#include <map>
#include <iostream>
#include <string>
#include <utility>

enum VirtualInputType {
	Raw    = 0,
	Range  = 1, // float [-1.0, 1.0]
	State  = 2, // 0 off, 1 on, 2 continue
	            // adapated for 1/0 values
	Action = 3  // bool
	            // adapated for 1/0 values
};

class VirtualInput
{
public:
	VirtualInput(VirtualInputType type) : type(type) {
		previousRawValue = 0;
		virtualValue = 0;
	}
	~VirtualInput() = default;
	
	void UpdateRawValue(float newValue) {
		previousRawValue = rawValue;
		rawValue = newValue;

		switch(type) {
			case VirtualInputType::State:
				if((virtualValue == 1.0f || virtualValue == 2.0f) && rawValue != 0.0f) {
					virtualValue = 2.0f;
				} else if (rawValue != 0.0f) {
					virtualValue = 1.0f;
				} else {
					virtualValue = 0.0f;
				}
				break;
			case VirtualInputType::Action:
				if(rawValue != 0.0f) {
					virtualValue = 1.0f;
				}
				else {
					virtualValue = 0.0f;
				}
				break;
			case VirtualInputType::Range:
				// clamp to range of [-1,1]
				if(rawValue > 1.0f) {
					virtualValue = 1.0f;
				} else 
				if(rawValue < -1.0f) {
					virtualValue = -1.0f;
				} else {
					virtualValue = rawValue;
				}
				break;
			case VirtualInputType::Raw:
				[[fallthrough]];
			default:
				virtualValue = rawValue;
				break;
		}
	}

	float GetValue() {
		return virtualValue;
	}

	VirtualInputType GetType() {
		return type;
	}
private:
	VirtualInputType type;
	float previousRawValue;
	float rawValue;
	float virtualValue;
};