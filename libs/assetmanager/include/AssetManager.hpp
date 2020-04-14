#pragma once

#include <memory>
#include <string>

class AssetManager
{
public:
	AssetManager();
	AssetManager(std::string assetPathPrefix);
	AssetManager(const char* assetPathPrefix);
	~AssetManager();

	std::string LoadTextFile(const std::string& path);
	std::string LoadTextFile(const char* path);

	char* LoadBinaryFile(const std::string& path, unsigned long& fileLen);
	char* LoadBinaryFile(const char* path, unsigned long& fileLen);

	// std::string GetTextFileContent(const std::string& path);
	// std::string GetTextFileContent(const char* path);

private:
	std::string assetPathPrefix = "";
};
