#include "enginecore.h"
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	EngineCore* Core;
	bool result;
	// Create the system object
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr); 

	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(hModule, path, MAX_PATH);

	std::wcout << path << std::endl;

	DWORD  nBufferLength = 512;
	char lpBuffer[512];
	GetCurrentDirectory(nBufferLength,lpBuffer);

	std::cout << lpBuffer << std::endl;

	Core = new EngineCore;
	if (!Core)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = Core->Init();
	if (result)
	{
		Core->Run();
	}

	// Shutdown and release the system object.
	if (Core != nullptr)
	{
		delete Core;
	}

	return 0;
}