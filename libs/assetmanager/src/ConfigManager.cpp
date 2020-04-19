#include "ConfigManager.hpp"

#include <iostream>
#include <sstream>

#include "Constants.h"

Settings ConfigManager::settingsFromFile;

ConfigManager::ConfigManager(std::string settingsFileName)
{
	std::shared_ptr<cpptoml::table> config;

	try {
		config = cpptoml::parse_file(settingsFileName);
	} catch (const cpptoml::parse_exception& e) {
		std::cerr << "Failed to parse " << settingsFileName << ": " << e.what()
		          << std::endl;
		std::cout << "Loading deafult settings instead..." << std::endl;

		SetToDefault();

		return;
	}

	SetSettingsFromParsedFile(config);
}

ConfigManager::~ConfigManager() {}

int
ConfigManager::SetToDefault()
{
	std::shared_ptr<cpptoml::table> config;

	char buffer[] = WARDROBE_DEFAULT_SETTINGS_STRING;

	membuf       sbuf(buffer, buffer + sizeof(buffer));
	std::istream in{ &sbuf };

	cpptoml::parser p{ in };

	config = p.parse();

	SetSettingsFromParsedFile(config);

	return 0;
}

void
ConfigManager::SetSettingsFromParsedFile(
  std::shared_ptr<cpptoml::table> parsedFile)
{
	settingsFromFile.borderless = *parsedFile->get_qualified_as<bool>(
	  "graphics.window.windowed_mode.borderless");
	settingsFromFile.resizable = *parsedFile->get_qualified_as<bool>(
	  "graphics.window.windowed_mode.resizable");
	settingsFromFile.vsync =
	  *parsedFile->get_qualified_as<bool>("graphics.vsync");

	auto sizeVector = *parsedFile->get_qualified_array_of<int64_t>(
	  "graphics.window.windowed_mode.default_size");

	settingsFromFile.size[0] = sizeVector[0];
	settingsFromFile.size[1] = sizeVector[1];

	std::string screenMode =
	  *parsedFile->get_qualified_as<std::string>("graphics.window.default_mode");

	screenMode == "fullscreen" ? settingsFromFile.fullscreen = true
	                           : settingsFromFile.fullscreen = false;

	settingsFromFile.cursorMode =
	  *parsedFile->get_qualified_as<std::string>("input.mouse.cursor_mode");
}