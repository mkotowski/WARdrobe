#pragma once

#include <iostream>
#include <map>

#include "ecs.hpp"

extern "C"
{
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "Character.hpp"

int
l_cppgetTransform(lua_State* l);

int a = 1, b = 2, c = 3;

struct Scripts
{
	std::list<std::string> names;
};

class ScriptsSystem : public System
{
	std::shared_ptr<ComponentManager> componentManager;
	lua_State*                        state;

	void ExecuteLuaFunction(std::string name, Entity e, int argsNr, int resNr);
	bool flag = false;

public:
	static std::shared_ptr<ComponentManager> componentManagerS;

	void Init(std::shared_ptr<ComponentManager> componentManager);
	void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void CloseAllLuaStates(std::shared_ptr<ComponentManager> componentManager);
};

std::shared_ptr<ComponentManager> ScriptsSystem::componentManagerS;

int
func(lua_State* l)
{
	lua_pushnumber(l, a);
	lua_pushnumber(l, b);
	lua_pushnumber(l, c);

	return 3;
}

int
funcU(lua_State* l)
{
	a = lua_tonumber(l, 1);
	b = lua_tonumber(l, 2);
	c = lua_tonumber(l, 3);

	return 1;
}

void
ScriptsSystem::Init(std::shared_ptr<ComponentManager> componentManager)
{
	/*scriptPreparationFunctions.insert(
	  { "characterMovement.lua",
	    std::make_unique<void (*)(
	      lua_State*, Entity, std::shared_ptr<ComponentManager>)>(
	      &prepFunctionForCharacterMovement) });*/

	this->componentManager = componentManager;
	componentManagerS = componentManager;

	state = luaL_newstate();
	luaL_openlibs(state);

	lua_pushcfunction(state, l_cppgetTransform);
	lua_setglobal(state, "getTransform");

	lua_pushlightuserdata(state, &(*componentManager));
	lua_setglobal(state, "componentManager");

	for (auto const& entity : entities) {
		auto& scripts = componentManager->GetComponent<Scripts>(entity);

		lua_pushnumber(state, entity);
		lua_setglobal(state, "entity");

		for (auto script : scripts.names) {

			if (script == "initActors.lua" && !flag) {
				flag = true;

				std::string filePath = "assets/scripts/" + script;

				if (luaL_loadfile(state, filePath.c_str())) {
					std::cerr << "Something went wrong loading the chunk (syntax error?)"
					          << std::endl;
					std::cerr << lua_tostring(state, -1) << std::endl;
					lua_pop(state, 1);
				}

				if (lua_pcall(state, 0, LUA_MULTRET, 0)) {
					std::cerr << "Something went wrong during execution" << std::endl;
					std::cerr << lua_tostring(state, -1) << std::endl;
					lua_pop(state, 1);
				}

				lua_pushcfunction(state, func);
				lua_setglobal(state, "testF");
				lua_pushcfunction(state, funcU);
				lua_setglobal(state, "testU");

				//ExecuteLuaFunction("start", entity);
				lua_getglobal(state, "start");
				/*if (lua_pcall(state, 0, 0, 0)) {
					std::cerr << "Something went wrong during execution" << std::endl;
					std::cerr << lua_tostring(state, -1) << std::endl;
					lua_pop(state, 1);
				}*/
				ExecuteLuaFunction("start", entity, 0, 0);

				std::cout << a << " " << b << " " << c << "\n";
			} else {
				std::string filePath = "assets/scripts/" + script;

				if (luaL_loadfile(state, filePath.c_str())) {
					std::cerr << "Something went wrong loading the chunk (syntax error?)"
					          << std::endl;
					std::cerr << lua_tostring(state, -1) << std::endl;
					lua_pop(state, 1);
				}

				if (lua_pcall(state, 0, LUA_MULTRET, 0)) {
					std::cerr << "Something went wrong during execution" << std::endl;
					std::cerr << lua_tostring(state, -1) << std::endl;
					lua_pop(state, 1);
				}

				//ExecuteLuaFunction("start", entity);
				lua_getglobal(state, "start");
				/*if (lua_pcall(state, 0, 0, 0)) {
					std::cerr << "Something went wrong during execution" << std::endl;
					std::cerr << lua_tostring(state, -1) << std::endl;
					lua_pop(state, 1);
				}*/
				ExecuteLuaFunction("start", entity, 0, 0);
			}
		}
	}
}

void
ScriptsSystem::Update(float                             dt,
                      std::shared_ptr<ComponentManager> componentManager)
{
	for (auto const& entity : entities) {
		auto& scripts = componentManager->GetComponent<Scripts>(entity);

		for (auto script : scripts.names) {
			if (script != "initActors.lua") {
				std::string filePath = "assets/scripts/" + script;

				if (luaL_loadfile(state, filePath.c_str())) {
					std::cerr << "Something went wrong loading the chunk (syntax error?)"
					          << std::endl;
					std::cerr << lua_tostring(state, -1) << std::endl;
					lua_pop(state, 1);
				}

				if (lua_pcall(state, 0, LUA_MULTRET, 0)) {
					std::cerr << "Something went wrong during execution" << std::endl;
					std::cerr << lua_tostring(state, -1) << std::endl;
					lua_pop(state, 1);
				}

				lua_getglobal(state, "update");
				lua_pushnumber(state, dt);
				ExecuteLuaFunction("update", entity, 1, 0);
			}
		}
	}
}

void
ScriptsSystem::ExecuteLuaFunction(std::string name, Entity e, int argsNr, int resNr)
{
	if (lua_pcall(state, argsNr, resNr, 0)) {
		std::cerr << "Something went wrong during execution" << std::endl;
		std::cerr << lua_tostring(state, -1) << std::endl;
		lua_pop(state, 1);
	}
}

void
ScriptsSystem::CloseAllLuaStates(
  std::shared_ptr<ComponentManager> componentManager)
{
	lua_close(state);
	state = nullptr;
}

int
l_cppgetTransform(lua_State* l)
{
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 1);

	Entity e = luaL_checknumber(l, 2);
	lua_pushlightuserdata(l, &cm->GetComponent<Transform>(e));
	return 1;
}