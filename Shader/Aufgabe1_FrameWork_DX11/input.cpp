
#include "input.h"
#include <windef.h>


Input::Input()
{
}

Input::Input(const Input &other)
{
}


Input::~Input()
{
}

bool Input::Init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;
	for (int i = 0; i < 256; i++)
	{
		keystate[i] = false;
		keystateOld[i] = false;
	}

	m_mouseX = 0;
	m_mouseY = 0;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	
	return true;
}

void Input::KeyDown(unsigned int i)
{
	keystate[i] = true;
	return;
}

void Input::KeyUp(unsigned int i)
{
	keystate[i] = false;
	return;
}

bool Input::Keystate(unsigned int i)
{
	return keystate[i];
}
bool Input::KeystateOld(unsigned int i)
{
	return keystateOld[i];
}
void Input::Swap()
{
	memcpy(keystateOld, keystate, 256 * sizeof(bool));
	memcpy(mousestateOld, mousestate, 1 * sizeof(bool));
}

void Input::ProcessInput()
{
	POINT p;
	if (GetCursorPos(&p))
	if (ScreenToClient(GetActiveWindow(), &p))
	{
		m_mouseX = p.x;
		m_mouseY = p.y;
	}
	mousePressed = false;;
}
