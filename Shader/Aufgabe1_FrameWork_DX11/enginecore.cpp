#include "enginecore.h"

EngineCore::EngineCore():
	inputObj(nullptr), graphObj(nullptr)
{
}

EngineCore::~EngineCore()
{
	delete graphObj;
	delete inputObj;
	ShutdownWindows();
}

bool EngineCore::Init()
{
	int screenWidth, screenHeight;
	bool result;


	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	inputObj = new Input;
	// Nullcheck
	if (!inputObj) return false;
	result = inputObj->Init(instance,hwnd,screenWidth,screenHeight);
	if (!result) return false;

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	graphObj = new GraphicsCore;
	// Nullcheck
	if (!graphObj) return false;
	result = graphObj->Init(screenWidth, screenHeight, hwnd);
	if (!result) return false;

	return true;
}

void EngineCore::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	previous_time = Time::now();
	while (!done)
	{
		current_time = Time::now();
		fsec delta_time = current_time - previous_time;
		//Message Handling
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Shutdown Case
		if (msg.message == WM_QUIT)	done = true;
		else
		{
			if (inputObj->Keystate(VK_RETURN) && !inputObj->KeystateOld(VK_RETURN))
			{
				if (!Editmode) Editmode = true;
				else Editmode = false;
			}
			// Process Frames      
			result = Frame(delta_time.count(),Editmode);
			if (!result) done = true;

		}
		inputObj->Swap();
		previous_time = current_time;
	}
}

bool EngineCore::Frame(float delta_time,bool Editmode)
{
	bool result;
	// Check if the user pressed escape and wants to exit the application.
	if (inputObj->Keystate(VK_ESCAPE)) return false;
	// Do the frame processing for the graphics object.
	result = graphObj->Frame(delta_time,inputObj,Editmode);
	if (!result) return false;

	//inputObj->Init();
	return true;
}

void EngineCore::InitWindows(int &screenWidth, int &screenHeight)
{
	WNDCLASSEX wc;
	int posX, posY;


	// Get an external pointer to this object.	
	appHandle = this;

	// Get the instance of this application.
	instance = GetModuleHandle(NULL);

	// Give the application a name.
	appName = "Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC ;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = appName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	//Set Resolution to 800x600
	screenWidth = 800;
	screenHeight = 600;

	//Set window to middle of the screen
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	
	RECT wr = { 0, 0, screenWidth, screenHeight };    // set the size, but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

	// Create the window with the screen settings and get the handle to it.
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, appName, appName,
		WS_SYSMENU | WS_OVERLAPPED | WS_CAPTION | WS_VISIBLE | WS_MINIMIZEBOX,
		posX, posY, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, instance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	return;
}

void EngineCore::ShutdownWindows()
{
	// Remove the window.
	DestroyWindow(hwnd);
	hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(appName, instance);
	instance = NULL;

	// Release the pointer to this class.
	appHandle = NULL;

	return;
}

LRESULT EngineCore::MsgHandle(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			// Send key to the input Obj for State proicessing
			inputObj->KeyDown((unsigned int)wparam);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			inputObj->LMouseDown();
			return 0;
		}
		case WM_LBUTTONUP:
		{
			inputObj->LMouseUp();
			return 0;
		}
		case WM_KEYUP:
		{
			// Send key to the input Obj for State proicessing
			inputObj->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Pass other Message on to windows Standard function
		default:
		{
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		case WM_DESTROY: case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Pass alle non quit Messages to the custom Message handler
		default:
		{
			return appHandle->MsgHandle(hwnd, umessage, wparam, lparam);
		}
	}
}
