#include <array>

#include "AssetManager.hpp"

AssetManager::AssetManager() {}

AssetManager::AssetManager(std::string assetPathPrefix)
  : assetPathPrefix(assetPathPrefix)
{}

AssetManager::AssetManager(const char* assetPathPrefix)
{
	this->assetPathPrefix = std::string(assetPathPrefix);
}

AssetManager::~AssetManager() {}

std::string
AssetManager::LoadTextFile(const std::string& path)
{
	return LoadTextFile(path.c_str());
}

// http://0x80.pl/notesen/2019-01-07-cpp-read-file.html
std::string
AssetManager::LoadTextFile(const char* path)
{
	std::string prefixedPath = std::string(assetPathPrefix + path);
	auto        close_file = [](FILE* f) { fclose(f); };

	auto holder = std::unique_ptr<FILE, decltype(close_file)>(
	  fopen(prefixedPath.c_str(), "rb"), close_file);
	if (!holder)
		return "";

	FILE* f = holder.get();

	// in C++17 following lines can be folded into std::filesystem::file_size
	// invocation
	if (fseek(f, 0, SEEK_END) < 0)
		return "";

	const long size = ftell(f);
	if (size < 0)
		return "";

	if (fseek(f, 0, SEEK_SET) < 0)
		return "";

	std::string res;
	res.resize(size);

	// C++17 defines .data() which returns a non-const pointer
	fread(const_cast<char*>(res.data()), 1, size, f);

	return res;
}

char *
AssetManager::LoadBinaryFile(const std::string& path, unsigned long& fileLen)
{
	return LoadBinaryFile(path.c_str(), fileLen);
}

char *
AssetManager::LoadBinaryFile(const char* path, unsigned long& fileLen)
{
	std::string prefixedPath = std::string(assetPathPrefix + path);

	FILE*         file;
	char*         buffer;
	//unsigned long fileLen;

	// Open file
	file = fopen(prefixedPath.c_str(), "rb");
	if (!file) {
		fprintf(stderr, "Unable to open file %s\n", prefixedPath.c_str());
		return nullptr;
	}

	// Get file length
	fseek(file, 0, SEEK_END);
	fileLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Allocate memory
	buffer = (char*)malloc(fileLen + 1);
	if (!buffer) {
		fprintf(stderr, "Memory error!");
		fclose(file);
		return nullptr;
	}

	// Read file contents into buffer
	fread(buffer, fileLen, 1, file);
	fclose(file);

	// Do what ever with buffer

	//free(buffer);

	//auto bufferPtr = std::make_shared<char[]>(fileLen);
	//auto bufferPtr = std::make_shared<char>(buffer);

	return buffer;
}