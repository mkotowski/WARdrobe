#pragma once

#include <iostream>

class Action
{
public:
	Action(const char* msg)
	  : msg(msg)
	{}
	~Action() {}
	const char* GetMessage() { return msg; }
	double      execute(double rawInput)
	{
		std::cout << msg << "\n";
		return rawInput;
	}

private:
	const char* msg;
};
