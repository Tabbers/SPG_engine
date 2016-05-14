#ifndef _ENFINECORE_H_
#define _ENFINECORE_H_

#include <windows.h>
#include <chrono>

#include "input.h"
#include "graphicscore.h"

class EngineCore
{
public:
	EngineCore();
	~EngineCore();

	bool Init();
	void Run();

	LRESULT CALLBACK MsgHandle(HWND, UINT, WPARAM, LPARAM);
private:
	bool Frame(float,bool);
	void InitWindows(int&, int&);
	void ShutdownWindows();
private:

	LPCSTR appName;
	HINSTANCE instance;
	HWND hwnd;
	bool Editmode = true;
	
	Input* inputObj;
	GraphicsCore* graphObj;

	typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<float> fsec;

	std::chrono::high_resolution_clock::time_point previous_time ;
	std::chrono::high_resolution_clock::time_point current_time;

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static EngineCore* appHandle = 0;
#endif

