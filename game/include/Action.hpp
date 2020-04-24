#pragma once

#include <iostream>

enum class ActionType : int
{
	Single = 0,
	State = 1,
	Range = 2
};

class Action
{
public:
	Action(const char* label, ActionType type)
	  : label(label)
	  , type(type)
	{
		rawValue = 0;
		isRawValueSet = false;
	}

	~Action() = default;
	const char* GetLabel() { return label; }
	void        execute(double rawInput)
	{
		rawValue = rawInput;
		std::cout << label << " action executed! Value: " << rawValue << "\n";
	}
	double     GetValue() { return rawValue; }
	ActionType GetType() { return type; }

private:
	const char* label;
	double      rawValue;
	bool        isRawValueSet;
	ActionType  type;
};
