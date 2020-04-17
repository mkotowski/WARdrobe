#pragma once

#include "cpptoml.h"

struct Settings
{
	bool vsync;
	bool resizable;
	bool borderless;
	bool fullscreen;
	int  size[2];
	std::string cursorMode;
};

class ConfigManager
{
public:
	ConfigManager(std::string settingsFileName);
	~ConfigManager();

	int SetToDefault();
	static inline Settings GetSettings() { return settingsFromFile; }

private:
	void SetSettingsFromParsedFile(std::shared_ptr<cpptoml::table> parsedFile);

	static Settings settingsFromFile;
};

struct membuf : std::streambuf
{
	membuf(char* begin, char* end) { this->setg(begin, begin, end); }
};
