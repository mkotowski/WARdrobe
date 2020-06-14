#pragma once

#include <iostream>
#include <map>

struct Scripts
{
	std::list<std::string> names;
};

class ScriptsSystem : public System
{
	std::shared_ptr<ComponentManager> componentManager;
	lua_State*                        state;
	std::list<std::string>            uniqueScripts;
	Window*                           ourWindow;

	void ExecuteLuaFunction(int argsNr, int resNr);
	bool IsScriptAlreadyInCollection(std::string name);

public:
	void Init(std::shared_ptr<ComponentManager> componentManager,
	          Window*                           w,
	          Camera*                           c);
	void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void CloseLuaState(std::shared_ptr<ComponentManager> componentManager);
};

bool
ScriptsSystem::IsScriptAlreadyInCollection(std::string name)
{
	for (std::string nameFromCollection : uniqueScripts) {
		if (nameFromCollection == name) {
			return true;
		}
	}

	return false;
}

void
ScriptsSystem::Init(std::shared_ptr<ComponentManager> componentManager,
                    Window*                           w,
                    Camera*                           c)
{
	ourWindow = w;

	state = luaL_newstate();
	luaL_openlibs(state);

	for (auto const& entity : entities) {
		auto& scripts = componentManager->GetComponent<Scripts>(entity);

		for (auto script : scripts.names) {

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
		}
	}

	this->componentManager = componentManager;

	lua_pushlightuserdata(state, &(*componentManager));
	lua_setglobal(state, "componentManager");

	lua_pushlightuserdata(state, &(*w));
	lua_setglobal(state, "window");

	lua_pushlightuserdata(state, &(*c));
	lua_setglobal(state, "camera");

	setAllFunctions(state);

	for (auto const& entity : entities) {
		auto& scripts = componentManager->GetComponent<Scripts>(entity);

		lua_pushnumber(state, entity);
		lua_setglobal(state, "entity");

		for (auto script : scripts.names) {

			std::string start = script.substr(0, script.length() - 4) + "Start";

			lua_getglobal(state, start.c_str());
			ExecuteLuaFunction(0, 0);
		}
	}
}

void
ScriptsSystem::Update(float                             dt,
                      std::shared_ptr<ComponentManager> componentManager)
{
	lua_pushnumber(state, playerInputHorizontal);
	lua_setglobal(state, "rightInput");
	lua_pushnumber(state, playerInputVertical);
	lua_setglobal(state, "forwardInput");
	lua_pushboolean(state, leftMousePressed);
	lua_setglobal(state, "leftMousePressed");
	lua_pushboolean(state, rightMousePressed);
	lua_setglobal(state, "rightMousePressed");
	lua_pushnumber(state, glfwGetTime());
	lua_setglobal(state, "time");

	for (auto const& entity : entities) {
		auto& scripts = componentManager->GetComponent<Scripts>(entity);

		lua_pushnumber(state, entity);
		lua_setglobal(state, "entity");

		for (auto script : scripts.names) {

			std::string update = script.substr(0, script.length() - 4) + "Update";

			lua_getglobal(state, update.c_str());
			lua_pushnumber(state, dt);
			ExecuteLuaFunction(1, 0);

			std::string onCollision =
			  script.substr(0, script.length() - 4) + "OnCollisionEnter";

			lua_getglobal(state, onCollision.c_str());
			bool exists = !lua_isnil(state, -1);
			if (exists) {
				for (Entity collidedEntity :
				     componentManager->GetComponent<BoundingBox>(entity)
				       .collisionEnterEntities) {

					lua_pushlightuserdata(
					  state, &componentManager->GetComponent<BoundingBox>(collidedEntity));
					ExecuteLuaFunction(1, 0);
				}
			} else {
				lua_pop(state, 1);
			}
		}
	}
}

void
ScriptsSystem::ExecuteLuaFunction(int argsNr, int resNr)
{
	if (lua_pcall(state, argsNr, resNr, 0)) {
		std::cerr << "Something went wrong during execution" << std::endl;
		std::cerr << lua_tostring(state, -1) << std::endl;
		lua_pop(state, 1);
	}
}

void
ScriptsSystem::CloseLuaState(std::shared_ptr<ComponentManager> componentManager)
{
	lua_close(state);
	state = nullptr;
}