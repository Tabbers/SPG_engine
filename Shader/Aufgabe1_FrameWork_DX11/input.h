#ifndef _INPUT_H_
#define _INPUT_H_

#include <string.h>
#include <dinput.h>

class Input
{
public:
	bool mousePressed = false;

	Input();
	Input(const Input&);
	~Input();
	
	bool Init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	inline void LMouseDown()
	{
		mousestate[0] = true;
	};
	inline void LMouseUp()
	{
		mousestate[0] = false;
	};
	inline void GetMouseCoord(int &mouseX, int &mouseY)
	{
		mouseX = m_mouseX;
		mouseY = m_mouseY;
	};
	inline void SetMouseCoord(int mouseX, int mouseY)
	{
		m_mouseX = mouseX;
		m_mouseY = mouseY;
	}

	bool Keystate(unsigned int);
	bool KeystateOld(unsigned int i);
	inline bool lMousestate()
	{
		return mousestate[0];
	};
	inline bool lMousestateOld()
	{
		return mousestateOld[0];
	};
	void ProcessInput();
	void Swap();

private:

	// enable keychek for every frame through saving key state values
	bool keystate[256];
	bool keystateOld[256];

	bool mousestate[1];
	bool mousestateOld[1];

	int m_mouseX, m_mouseY;
	int m_screenWidth, m_screenHeight;
};
#endif

