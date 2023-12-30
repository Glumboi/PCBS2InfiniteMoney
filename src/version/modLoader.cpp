#include "ModLoader.h"
#include "debugUtils.h"

using PluginEntryPoint = void(*)();

ModLoader::ModLoader() {
	ModLoader::Init();
}

void ModLoader::Init()
{
	LoadConfig("./GlummyLoader.cfg");
	this->LoadAllPlugins();
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)KeyboardHandler::KeyBoardLoop, this, 0, 0);
}

void ModLoader::LoadConfig(const std::string fileName)
{
	//Create a small config file
	if (!std::filesystem::exists(fileName))
	{
		std::ofstream cfgFile(fileName);
		cfgFile << "LoadASI=0";
		cfgFile.close();
	}

	std::ifstream cfgFileIn(fileName);
	if (cfgFileIn.good())
	{
		std::string sLine;
		getline(cfgFileIn, sLine);
		size_t lastEq = sLine.find('=') + 1;
		std::string toggleSbStr = sLine.substr(lastEq, sLine.length() - lastEq);
		TimeStampDebug("LOAD ASI: " + toggleSbStr);
		this->skipASI = toggleSbStr == "0";
	}
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
		std::filesystem::path extension = entry.path().extension();
		if (extension == ".dll") {
			LoadPlugin(entry.path().string());
		}
		else if (extension == ".asi")
		{
			if (!this->skipASI) {
				LoadPlugin(entry.path().string());
			}
		}
	}
}

void ModLoader::UnloadAllPlugins() {
	for (HMODULE hModule : this->loadedPlugins) {
		FreeLibrary(hModule);
	}
	this->loadedPlugins.clear();
}

void KeyboardHandler::KeyBoardLoop(ModLoader* loaderInstance) {
	while (true)
	{
		if (GetKeyState(VK_F9) KEYISPRESSED)
		{
			TimeStampDebug("F9 pressed, reloading plugins!");
			loaderInstance->UnloadAllPlugins();
			loaderInstance->LoadAllPlugins();
			TimeStampDebug("Plugins reloaded!");
			Sleep(400);
		}
	}
}