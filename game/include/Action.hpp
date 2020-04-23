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
	void        execute() { std::cout << msg << "\n"; }

private:
	const char* msg;
};
