#pragma once

#pragma comment(linker,"/export:GetFileVersionInfoA=C:\\Windows\\System32\\version.GetFileVersionInfoA,@1")
#pragma comment(linker,"/export:GetFileVersionInfoByHandle=C:\\Windows\\System32\\version.GetFileVersionInfoByHandle,@2")
#pragma comment(linker,"/export:GetFileVersionInfoExA=C:\\Windows\\System32\\version.GetFileVersionInfoExA,@3")
#pragma comment(linker,"/export:GetFileVersionInfoExW=C:\\Windows\\System32\\version.GetFileVersionInfoExW,@4")
#pragma comment(linker,"/export:GetFileVersionInfoSizeA=C:\\Windows\\System32\\version.GetFileVersionInfoSizeA,@5")
#pragma comment(linker,"/export:GetFileVersionInfoSizeExA=C:\\Windows\\System32\\version.GetFileVersionInfoSizeExA,@6")
#pragma comment(linker,"/export:GetFileVersionInfoSizeExW=C:\\Windows\\System32\\version.GetFileVersionInfoSizeExW,@7")
#pragma comment(linker,"/export:GetFileVersionInfoSizeW=C:\\Windows\\System32\\version.GetFileVersionInfoSizeW,@8")
#pragma comment(linker,"/export:GetFileVersionInfoW=C:\\Windows\\System32\\version.GetFileVersionInfoW,@9")
#pragma comment(linker,"/export:VerFindFileA=C:\\Windows\\System32\\version.VerFindFileA,@10")
#pragma comment(linker,"/export:VerFindFileW=C:\\Windows\\System32\\version.VerFindFileW,@11")
#pragma comment(linker,"/export:VerInstallFileA=C:\\Windows\\System32\\version.VerInstallFileA,@12")
#pragma comment(linker,"/export:VerInstallFileW=C:\\Windows\\System32\\version.VerInstallFileW,@13")
#pragma comment(linker,"/export:VerLanguageNameA=C:\\Windows\\System32\\version.VerLanguageNameA,@14")
#pragma comment(linker,"/export:VerLanguageNameW=C:\\Windows\\System32\\version.VerLanguageNameW,@15")
#pragma comment(linker,"/export:VerQueryValueA=C:\\Windows\\System32\\version.VerQueryValueA,@16")
#pragma comment(linker,"/export:VerQueryValueW=C:\\Windows\\System32\\version.VerQueryValueW,@17")

#include <iostream>

#include "windows.h"
#include "ios"
#include "fstream"
#include <MinHook.h>
#include <string>
#include <filesystem>
#include "offsets.h"
#include "debugUtils.h"

// Remove this line if you aren't proxying any functions.
HMODULE hModule = LoadLibrary("C:\\Windows\\System32\\version.dll");
HANDLE prHandle = GetCurrentProcess();

// Function pointer type for the plugin entry point
using PluginEntryPoint = void(*)();
std::vector<HMODULE> loadedPlugins;  // Keep track of loaded plugins

void Init()
{
	InitConsole();

	//Load base address of GameAssembly.dll
	gameAssembly = reinterpret_cast<uintptr_t>(GetModuleHandle("GameAssembly.dll"));

	MH_Initialize();
	TimeStampDebug("Initialized MinHook");
	TimeStampDebug("GameAssembly.dll base address: " + std::to_string(gameAssembly));
	//std::cout << "GameAssembly.dll base address: " << std::to_string(gameAssembly) << std::endl;
}

void LoadPlugin(const std::string& path) {
	HMODULE hModule = LoadLibrary(path.c_str());
	if (hModule == nullptr) {
		DWORD error = GetLastError();
		PutDebug("Failed to load library " + path + " with error: " + (char*)error);
	}
	else {
		loadedPlugins.push_back(hModule);
		PutDebug("Loaded library: " + path);

		// Retrieve and call the entry point function if needed
		PluginEntryPoint entryPoint = reinterpret_cast<PluginEntryPoint>(GetProcAddress(hModule, "PluginEntryPoint"));
		if (entryPoint != nullptr) {
			entryPoint();  // Call the entry point function
		}
	}
}

void LoadPlugins() {
	char buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	std::string pluginFolder = std::string(buffer) + "\\Plugins";

	if (!std::filesystem::exists(pluginFolder)) {
		PutDebug("Plugins folder does not exist.");
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(pluginFolder)) {
		if (entry.path().extension() == ".dll") {
			LoadPlugin(entry.path().string());
		}
	}
}

void UnloadPlugins() {
	for (HMODULE hModule : loadedPlugins) {
		FreeLibrary(hModule);
	}
	loadedPlugins.clear();
}

void Main()
{
	Init();
	LoadPlugins();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Main, 0, 0, 0);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}