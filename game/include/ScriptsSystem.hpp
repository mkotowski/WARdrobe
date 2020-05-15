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

struct Scripts
{
	std::list<std::string> names;
};

class ScriptsSystem : public System
{
	std::map<std::string,
	         std::unique_ptr<
	           void (*)(lua_State*, Entity, std::shared_ptr<ComponentManager>)>>
	  scriptPreparationFunctions;

public:
	void Init();
	void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void ExecuteLuaScript(std::string                       name,
	                      Entity                            entity,
	                      std::shared_ptr<ComponentManager> componentManager);
};

void
prepFunctionForCharacterMovement(lua_State*,
                                 Entity,
                                 std::shared_ptr<ComponentManager>);

void
ScriptsSystem::Init()
{
	/*scriptPreparationFunctions.insert(
	  { "characterMovement.lua",
	    std::make_unique<void (*)(
	      lua_State*, Entity, std::shared_ptr<ComponentManager>)>(
	      &prepFunctionForCharacterMovement) });*/
}

void
ScriptsSystem::Update(float                             dt,
                      std::shared_ptr<ComponentManager> componentManager)
{
	for (auto const& entity : entities) {
		auto& scripts = componentManager->GetComponent<Scripts>(entity);

		/*for (auto script : scripts.names) {
			if (scriptPreparationFunctions.find(script) !=
			    scriptPreparationFunctions.end()) {
				ExecuteLuaScript(script, entity, componentManager);
			} else {
				std::cout << "Script with name " + script << " not found!" << std::endl;
			}
		}*/
	}
}

void
ScriptsSystem::ExecuteLuaScript(
  std::string                       name,
  Entity                            entity,
  std::shared_ptr<ComponentManager> componentManager)
{
	/////Initialization
	lua_State* L;
	L = luaL_newstate();
	luaL_openlibs(L);
	std::string filePath = "assets/scripts/" + name;
	if (luaL_loadfile(L, filePath.c_str())) {
		std::cerr << "Something went wrong loading the chunk (syntax error?)"
		          << std::endl;
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}

	(*scriptPreparationFunctions.find(name)->second)(L, entity, componentManager);

	if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
		std::cerr << "Something went wrong during execution" << std::endl;
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}

	lua_close(L);
}

int
l_cppCharacterMove(lua_State* l)
{
	Character* ch = (Character*)lua_touserdata(l, 1);
	RigidBody* rb = (RigidBody*)lua_touserdata(l, 2);
	float      x = luaL_checknumber(l, -1);
	float      y = luaL_checknumber(l, -2);
	float      z = luaL_checknumber(l, -3);
	float      speed = luaL_checknumber(l, -4);
	ch->Move(rb, x, y, z, speed);
	return 1;
}

void
prepFunctionForCharacterMovement(
  lua_State*                        L,
  Entity                            entity,
  std::shared_ptr<ComponentManager> componentManager)
{
	lua_pushcfunction(L, l_cppCharacterMove);
	lua_setglobal(L, "CharacterMove");

	lua_pushlightuserdata(L, &componentManager->GetComponent<Character>(entity));
	lua_setglobal(L, "character");

	lua_pushlightuserdata(L, &componentManager->GetComponent<RigidBody>(entity));
	lua_setglobal(L, "rigidbody");
}