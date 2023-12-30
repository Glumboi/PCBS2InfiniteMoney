#pragma once
#include "fstream"
#include "ios"
#include <direct.h>
#include <filesystem>
#include <iomanip>  // For std::setw
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <winnt.h>

#define KEYISPRESSED < 0
#define KEYISNOTPRESSED > 0

enum ModLoaderInitState { //--> implement at some point
	UNDEFINED,
	SUCC,
	ERR,
	WARN
};

class ModLoader
{
public:
	static HANDLE gameHandle;
	ModLoader();
	void Init();
	void LoadAllPlugins();
	void UnloadAllPlugins();

private:
	std::string pluginsPath;
	std::vector<HMODULE> loadedPlugins;
	void LoadPlugin(const std::string& path);
	void KeyBoardLoop();
};
