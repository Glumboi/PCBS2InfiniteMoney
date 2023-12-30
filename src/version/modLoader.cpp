#include "ModLoader.h"
#include "debugUtils.h"

using PluginEntryPoint = void(*)();

ModLoader::ModLoader() {
	ModLoader::Init();
}

void ModLoader::Init()
{
	this->LoadAllPlugins();
	KeyBoardLoop();
}

void ModLoader::LoadPlugin(const std::string& path) {
	HMODULE hModule = LoadLibrary(path.c_str());
	if (hModule == nullptr) {
		DWORD error = GetLastError();
		PutDebug("Failed to load library " + path + " with error: " + (char*)error);
	}
	else {
		this->loadedPlugins.push_back(hModule);
		PutDebug("Loaded library: " + path);

		// Retrieve and call the entry point function if needed
		PluginEntryPoint entryPoint = reinterpret_cast<PluginEntryPoint>(GetProcAddress(hModule, "PluginEntryPoint"));
		if (entryPoint != nullptr) {
			entryPoint();  // Call the entry point function
		}
	}
}

void ModLoader::LoadAllPlugins() {
	char buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	pluginsPath = std::string(buffer) + "\\Plugins";

	if (!std::filesystem::exists(pluginsPath)) {
		PutDebug("Plugins folder does not exist, creating a new one.");
		_mkdir(pluginsPath.c_str());
	}

	for (const auto& entry : std::filesystem::directory_iterator(pluginsPath)) {
		if (entry.path().extension() == ".dll") {
			LoadPlugin(entry.path().string());
		}
	}
}

void ModLoader::UnloadAllPlugins() {
	for (HMODULE hModule : this->loadedPlugins) {
		FreeLibrary(hModule);
	}
	this->loadedPlugins.clear();
}

void ModLoader::KeyBoardLoop() {
	while (true)
	{
		if (GetKeyState(VK_F9) KEYISPRESSED)
		{
			TimeStampDebug("F9 pressed!");
		}
	}
}