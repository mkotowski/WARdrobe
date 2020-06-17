#pragma once

extern "C"
{
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#pragma region Transform
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

	float x = luaL_checknumber(l, 3);
	float y = luaL_checknumber(l, 4);
	float z = luaL_checknumber(l, 5);

	cm->GetComponent<Transform>(e).position = glm::vec3(x, y, z);

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

	float x = luaL_checknumber(l, 3);
	float y = luaL_checknumber(l, 4);
	float z = luaL_checknumber(l, 5);

	cm->GetComponent<Transform>(e).rotation = glm::vec3(x, y, z);

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
	model = glm::scale(model, cm->GetComponent<Transform>(e).scale);

	glm::mat4 finalModel = parentModel * model;

	cm->GetComponent<Transform>(e).position =
	  glm::vec3(finalModel[3][0], finalModel[3][1], finalModel[3][2]);

	cm->GetComponent<Transform>(e).rotation = glm::vec3(rot.x, rot.y, rot.z);

	return 1;
}
#pragma endregion

#pragma region Rigidbody
int
l_cppgetRBVelocity(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	lua_pushnumber(l, cm->GetComponent<RigidBody>(e).velocity.x);
	lua_pushnumber(l, cm->GetComponent<RigidBody>(e).velocity.y);
	lua_pushnumber(l, cm->GetComponent<RigidBody>(e).velocity.z);

	return 3;
}

int
l_cppsetRBVelocity(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	float x = luaL_checknumber(l, 3);
	float y = luaL_checknumber(l, 4);
	float z = luaL_checknumber(l, 5);

	cm->GetComponent<RigidBody>(e).velocity = glm::vec3(x, y, z);

	return 1;
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

	cm->GetComponent<RigidBody>(e).applyForce(glm::normalize(glm::vec3(x, y, z)) *
	                                          f);

	return 1;
}
#pragma endregion

#pragma region Other
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
#pragma endregion

#pragma region BoundingBox
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
l_cppgetBoundingBoxDepth(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	lua_pushnumber(l, cm->GetComponent<BoundingBox>(e).depth);

	return 1;
}

int
l_cppsetBoundingBoxDepth(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	float defaultd = luaL_checknumber(l, 3);
	float d = luaL_checknumber(l, 4);

	cm->GetComponent<BoundingBox>(e).depth = d;
	cm->GetComponent<Transform>(e).scale.z =
	  cm->GetComponent<BoundingBox>(e).orgScale.z * d / defaultd;

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
#pragma endregion

#pragma region Renderer
int
l_cppsetColor(lua_State* l)
{
	Entity            e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	float r = luaL_checknumber(l, 3);
	float g = luaL_checknumber(l, 4);
	float b = luaL_checknumber(l, 5);

	cm->GetComponent<Renderer>(e).newColor = glm::vec3(r, g, b);

	return 1;
}
#pragma endregion

#pragma region Entity
int
l_cppdestroyEntity(lua_State* l)
{
	GameplayManager* gm = (GameplayManager*)lua_touserdata(l, 1);
	Entity           e = luaL_checknumber(l, 2);

	gm->DestroyEntity(e);

	return 1;
}
#pragma endregion

#pragma region Animator
int
l_cppplayAnimation(lua_State* l)
{
	Entity           e = luaL_checknumber(l, 1);
	ComponentManager* cm = (ComponentManager*)lua_touserdata(l, 2);

	std::string a = lua_tostring(l, 3);

	cm->GetComponent<Animator>(e).ChangeAnimation(a);

	return 1;
}
#pragma endregion


void
setAllFunctions(lua_State* state)
{
	// Transform
	lua_pushcfunction(state, l_cppgetTransform);
	lua_setglobal(state, "getTransform");

	lua_pushcfunction(state, l_cppsetTransform);
	lua_setglobal(state, "setTransform");

	lua_pushcfunction(state, l_cppgetRotation);
	lua_setglobal(state, "getRotation");

	lua_pushcfunction(state, l_cppsetRotation);
	lua_setglobal(state, "setRotation");

	lua_pushcfunction(state, l_cppsetTransformRelToRotatingParent);
	lua_setglobal(state, "setTransformRelToRotatingParent");

	// Rigidbody
	lua_pushcfunction(state, l_cppgetRBVelocity);
	lua_setglobal(state, "getVelocity");

	lua_pushcfunction(state, l_cppsetRBVelocity);
	lua_setglobal(state, "setVelocity");

	lua_pushcfunction(state, l_cppapplyForce);
	lua_setglobal(state, "applyForce");

	// Other
	lua_pushcfunction(state, l_cppgetMouseWorldPos);
	lua_setglobal(state, "getMouseWorldPos");

	// BoundingBox
	lua_pushcfunction(state, l_cppsetBoundingBox);
	lua_setglobal(state, "setBoundingBox");

	lua_pushcfunction(state, l_cppgetBoundingBoxDepth);
	lua_setglobal(state, "getBoundingBoxDepth");

	lua_pushcfunction(state, l_cppsetBoundingBoxDepth);
	lua_setglobal(state, "setBoundingBoxDepth");

	lua_pushcfunction(state, l_cppgetTag);
	lua_setglobal(state, "getTag");

	lua_pushcfunction(state, l_cppgetBoxPosition);
	lua_setglobal(state, "getBoxPosition");

	// Model
	lua_pushcfunction(state, l_cppsetColor);
	lua_setglobal(state, "setColor");

	// Entity
	lua_pushcfunction(state, l_cppdestroyEntity);
	lua_setglobal(state, "destroyEntity");

	// Animator
	lua_pushcfunction(state, l_cppplayAnimation);
	lua_setglobal(state, "playAnimation");
}