#pragma once

#include <iostream>

class Action
{
public:
	Action(const char* label)
	  : label(label)
	{}

	Action(const Action& action)
	  : label(action.label)
	{}

	~Action() = default;
	const char* GetLabel() { return label; }
	double      execute(double rawInput)
	{
		std::cout << label << " action executed!\n";
		return rawInput;
	}

private:
	const char* label;
};
