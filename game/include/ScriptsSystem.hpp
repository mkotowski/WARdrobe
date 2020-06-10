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
int
l_cppsetTransform(lua_State* l);
int
l_cppsetRBVelocity(lua_State* l);
int
l_cppgetMouseWorldPos(lua_State* l);
int
l_cppgetRotation(lua_State* l);
int
l_cppsetRotation(lua_State* l);
int
l_cppsetTransformRelToRotatingParent(lua_State* l);
int
l_cppsetBoundingBox(lua_State* l);
int
l_cppgetTag(lua_State* l);
int
l_cppgetBoxPosition(lua_State* l);
int
l_cppapplyForce(lua_State* l);

int a = 1, b = 2, c = 3;

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
	bool flag = false;

public:
	void Init(std::shared_ptr<ComponentManager> componentManager,
	          Window*                           w,
	          Camera*                           c);
	void Update(float                             dt,
	            std::shared_ptr<ComponentManager> componentManager) override;
	void CloseLuaState(std::shared_ptr<ComponentManager> componentManager);
};

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

	lua_pushcfunction(state, l_cppgetTransform);
	lua_setglobal(state, "getTransform");

	lua_pushcfunction(state, l_cppsetTransform);
	lua_setglobal(state, "setTransform");

	lua_pushcfunction(state, l_cppsetTransformRelToRotatingParent);
	lua_setglobal(state, "setTransformRelToRotatingParent");

	lua_pushcfunction(state, l_cppgetRotation);
	lua_setglobal(state, "getRotation");

	lua_pushcfunction(state, l_cppsetRotation);
	lua_setglobal(state, "setRotation");

	lua_pushcfunction(state, l_cppsetRBVelocity);
	lua_setglobal(state, "setVelocity");

	lua_pushcfunction(state, l_cppapplyForce);
	lua_setglobal(state, "applyForce");

	lua_pushcfunction(state, l_cppgetMouseWorldPos);
	lua_setglobal(state, "getMouseWorldPos");

	lua_pushcfunction(state, l_cppsetBoundingBox);
	lua_setglobal(state, "setBoundingBox");

	lua_pushcfunction(state, l_cppgetTag);
	lua_setglobal(state, "getTag");

	lua_pushcfunction(state, l_cppgetBoxPosition);
	lua_setglobal(state, "getBoxPosition");

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
	lua_pushnumber(state, glfwGetTime());
	lua_setglobal(state, "time");

	for (auto const& entity : entities) {
		auto& scripts = componentManager->GetComponent<Scripts>(entity);

		lua_pushnumber(state, entity);
		lua_setglobal(state, "entity");

		for (auto script : scripts.names) {

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

			std::string update = script.substr(0, script.length() - 4) + "Update";

			lua_getglobal(state, update.c_str());
			lua_pushnumber(state, dt);
			ExecuteLuaFunction(1, 0);
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

int
l_cppgetTransform(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	Transform t = cm->GetComponent<Transform>(e);

	lua_pushnumber(l, t.position.x);
	lua_pushnumber(l, t.position.y);
	lua_pushnumber(l, t.position.z);

	return 3;
}

int
l_cppsetTransform(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	Transform* t = &cm->GetComponent<Transform>(e);

	float x = luaL_checknumber(l, 3);
	float y = luaL_checknumber(l, 4);
	float z = luaL_checknumber(l, 5);

	t->position.x = x;
	t->position.y = y;
	t->position.z = z;

	return 1;
}

int
l_cppgetRotation(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	Transform* t = &cm->GetComponent<Transform>(e);

	float x = t->rotation.x;
	float y = t->rotation.y;
	float z = t->rotation.z;

	lua_pushnumber(l, x);
	lua_pushnumber(l, y);
	lua_pushnumber(l, z);

	return 3;
}

int
l_cppsetRotation(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	Transform* t = &cm->GetComponent<Transform>(e);

	float x = luaL_checknumber(l, 3);
	float y = luaL_checknumber(l, 4);
	float z = luaL_checknumber(l, 5);

	t->rotation.x = x;
	t->rotation.y = y;
	t->rotation.z = z;

	return 3;
}

int
l_cppsetTransformRelToRotatingParent(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	glm::vec4 playerPos = glm::vec4(luaL_checknumber(l, 3),
	                                luaL_checknumber(l, 4),
	                                luaL_checknumber(l, 5),
	                                0.0f);
	glm::vec4 pos = glm::vec4(luaL_checknumber(l, 6),
	                          luaL_checknumber(l, 7),
	                          luaL_checknumber(l, 8),
	                          0.0f);
	glm::vec3 rot = glm::vec3(
	  luaL_checknumber(l, 9), luaL_checknumber(l, 10), luaL_checknumber(l, 11));

	glm::mat4 parentModel = glm::mat4(1.0f);

	parentModel = glm::translate(parentModel, glm::vec3(playerPos));
	parentModel =
	  glm::rotate(parentModel, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	parentModel =
	  glm::rotate(parentModel, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	parentModel =
	  glm::rotate(parentModel, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(pos));

	glm::mat4 finalModel = parentModel * model;

	Transform* t = &cm->GetComponent<Transform>(e);

	t->position.x = finalModel[3][0];
	t->position.y = finalModel[3][1];
	t->position.z = finalModel[3][2];
	t->rotation.x = rot.x;
	t->rotation.y = rot.y;
	t->rotation.z = rot.z;

	return 1;
}

int
l_cppsetCameraTransform(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	Camera t = cm->GetComponent<Camera>(e);

	float x = luaL_checknumber(l, 3);
	float y = luaL_checknumber(l, 4);
	float z = luaL_checknumber(l, 5);

	t.cameraPos.x = x;
	t.cameraPos.y = y;
	t.cameraPos.z = z;

	return 1;
}

int
l_cppsetRBVelocity(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	float x = luaL_checknumber(l, 3);
	float y = luaL_checknumber(l, 4);
	float z = luaL_checknumber(l, 5);

	RigidBody* t = &cm->GetComponent<RigidBody>(e);
	t->velocity.x = x;
	t->velocity.y = y;
	t->velocity.z = z;

	return 1;
}

int
l_cppgetMouseWorldPos(lua_State* l)
{
	Window* w = (Window*)lua_touserdata(l, 1);
	Camera* sceneCamera = (Camera*)lua_touserdata(l, 2);

	double x, y;
	glfwGetCursorPos(w->GetWindow(), &x, &y);

	glm::mat4 projection =
	  glm::perspective(glm::radians(80.0f),
	                   (float)w->GetWindowWidth() / (float)w->GetWindowHeight(),
	                   0.1f,
	                   100.0f);
	glm::mat4 view =
	  glm::lookAt(sceneCamera->cameraPos,
	              sceneCamera->cameraPos + sceneCamera->cameraFront,
	              sceneCamera->cameraUp);

	glm::mat4 PVinv = glm::inverse(view * projection);

	glm::vec4 pos = glm::vec4(2.0f * (x / w->GetWindowWidth()) - 1.0f,
	                          1.0f - (2.0f * (y / w->GetWindowHeight())),
	                          1.0f,
	                          1.0f);

	pos = pos * PVinv;

	lua_pushnumber(l, pos.x);
	lua_pushnumber(l, pos.z);

	return 2;
}

int
l_cppsetBoundingBox(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);
	bool              set = lua_toboolean(l, 3);

	cm->GetComponent<BoundingBox>(e).enabled = set;

	return 1;
}

int
l_cppgetTag(lua_State* l)
{
	BoundingBox* bb = (BoundingBox*)lua_touserdata(l, 1);

	lua_pushstring(l, bb->tag.c_str());

	return 1;
}

int
l_cppgetBoxPosition(lua_State* l)
{
	BoundingBox* bb = (BoundingBox*)lua_touserdata(l, 1);

	lua_pushnumber(l, bb->origin.x);
	lua_pushnumber(l, bb->origin.y);
	lua_pushnumber(l, bb->origin.z);

	return 3;
}

int
l_cppapplyForce(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);
	float             x = luaL_checknumber(l, 3);
	float             y = luaL_checknumber(l, 4);
	float             z = luaL_checknumber(l, 5);
	float             f = luaL_checknumber(l, 6);

	cm->GetComponent<RigidBody>(e).applyForce(glm::normalize(glm::vec3(x, y, z)) * f);

	return 1;
}