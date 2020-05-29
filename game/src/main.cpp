#include <iostream>
#include <mutex>
#include <thread>

//#include <ctime>

#include <clocale>

#include <nlohmann/json.hpp>

extern "C"
{
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "AssetManager.hpp"
#include "AudioManager.hpp"
#include "ConfigManager.hpp"
#include "Game.hpp"

#include "Constants.h"

#include "Logger.hpp"

extern "C"
{
	static int l_cppfunction(lua_State* L)
	{
		double arg = luaL_checknumber(L, 1);
		lua_pushnumber(L, arg * 0.5);
		return 1;
	}
}

std::mutex mu;

void
shared_cout(std::string msg, int id)
{
	mu.lock();
	std::cout << msg << ":" << id << std::endl;
	mu.unlock();
}

void
thread_function()
{
	for (int i = -100; i < 0; i++) {
		shared_cout("thread function", i);
	}
}

void
call_from_thread(std::shared_ptr<AssetManager> assetManager)
{
	AudioManager* a = new AudioManager(assetManager);
	a->demo();
}

void
call_render()
{
	Game* game = new Game("Test nazwa");
	game->Loop();
}

void
luaTest()
{
	std::string  path = "scripts/luascript.lua";
	AssetManager a("assets/");
	std::string  aa = a.LoadTextFile(path);
	std::cout << aa << std::endl;

	std::cout << std::endl << WARDROBE_DEFAULT_SETTINGS_STRING << std::endl;

	unsigned long size;
	auto          b = a.LoadBinaryFile("audio/footsteps.raw", size);
	std::cout << "file size: " << size
	          << " and first int value: " << static_cast<int>(b[0]) << std::endl;
	free(b);

	std::cout << "** Test Lua embedding" << std::endl;
	std::cout << "** Init Lua" << std::endl;
	/////Initialization
	lua_State* L;
	L = luaL_newstate();
	std::cout
	  << "** Load the (optional) standard libraries, to have the print function"
	  << std::endl;
	luaL_openlibs(L);
	std::cout << "** Load chunk. without executing it" << std::endl;
	if (luaL_loadfile(L, "assets/scripts/luascript.lua")) {
		std::cerr << "Something went wrong loading the chunk (syntax error?)"
		          << std::endl;
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}

	/////Adding variables from C++ into Lua
	std::cout << "** Make a insert a global var into Lua from C++" << std::endl;
	lua_pushnumber(L, 1.1);
	lua_setglobal(L, "cppvar");

	std::cout << "** Execute the Lua chunk" << std::endl;
	if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
		std::cerr << "Something went wrong during execution" << std::endl;
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}

	////Reading a Lua variable from C++
	std::cout << "** Read a global var from Lua into C++" << std::endl;
	lua_getglobal(L, "luavar");
	double luavar = lua_tonumber(L, -1);
	lua_pop(L, 1);
	std::cout << "C++ can read the value set from Lua luavar = " << luavar
	          << std::endl;

	std::cout << "** Execute a Lua function from C++" << std::endl;
	lua_getglobal(L, "myluafunction");
	lua_pushnumber(L, 5);
	lua_pcall(L, 1, 1, 0);
	std::cout << "The return value of the function was " << lua_tostring(L, -1)
	          << std::endl;
	lua_pop(L, 1);

	std::cout << "** Execute a C++ function from Lua" << std::endl;
	std::cout << "**** First register the function in Lua" << std::endl;
	lua_pushcfunction(L, l_cppfunction);
	lua_setglobal(L, "cppfunction");

	std::cout << "**** Call a Lua function that uses the C++ function"
	          << std::endl;
	lua_getglobal(L, "myfunction");
	lua_pushnumber(L, 5);
	lua_pcall(L, 1, 1, 0);
	std::cout << "The return value of the function was " << lua_tonumber(L, -1)
	          << std::endl;
	lua_pop(L, 1);

	std::cout << "** Release the Lua enviroment" << std::endl;
	lua_close(L);
}

int
main()
{
	std::setlocale(LC_ALL, "pl_PL");

	Logger log;

	//std::setlocale(LC_ALL, "pl_PL.UTF-8");
	//std::setlocale(LC_NUMERIC, "de_DE.UTF-8");
	//std::setlocale(LC_TIME, "ja_JP.UTF-8");

	/*wchar_t     str[100];
	std::time_t t = std::time(NULL);
	std::wcsftime(str, 100, L"%A %c", std::localtime(&t));
	std::wprintf(L"Number: %.2f\nDate: %Ls\n", 3.14, str);*/

	using json = nlohmann::json;

	luaTest();

	std::shared_ptr<AssetManager> assetManager = std::make_shared<AssetManager>();
	std::shared_ptr<ConfigManager> configManager = std::make_shared<ConfigManager>("settings.toml");

	/*std::thread t1(call_from_thread, std::ref(assetManager));
	std::thread t2(call_render);
	std::thread t3(thread_function);*/

	/*for (int i = 100; i > 0; i--) {
		shared_cout("main thread", i);
	}*/

	/*if (t1.joinable())
		t1.join();
	if (t2.joinable())
		t2.join();
	if (t3.joinable())
		t3.join();*/

	

	Game* game = new Game("WARdrobe");
	game->Loop();

	//AudioManager* a = new AudioManager(assetManager);
	//a->demo();

	return 0;
}
