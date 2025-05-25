#include "Engine.hpp"
#include <iostream>
#include <fstream>

#ifdef ERROR
#undef ERROR
#endif

Engine::Engine() : m_application(nullptr), m_input(nullptr), m_hinstance(nullptr), m_hwnd (nullptr)
{
#ifndef NDEBUG
	CreateConsole();
#endif
	if (!Initialize(1280, 720, false))
	{
		Logger::Log(LogLevel::ERROR, "Failed to initialize window.");
		MessageBox(m_hwnd, L"Failed to initialize Window", L"Error", MB_OK);
		DestroyWindow(m_hwnd);
	}
	Logger::Log(INFO, "Application started");
}

Engine::Engine(int width, int height, bool fullscreen) : m_application(nullptr), m_input(nullptr), m_hinstance(nullptr), m_hwnd(nullptr)
{
#ifndef NDEBUG
	CreateConsole();
#endif
	if (!Initialize(width, height, fullscreen))
	{
		Logger::Log(LogLevel::ERROR, "Failed to initialize window.");
	}
	Logger::Log(INFO, "Application started");
}

Engine::Engine(const Engine&) : m_application(nullptr), m_input(nullptr), m_hinstance(nullptr), m_hwnd(nullptr)
{
}

Engine::~Engine()
{
}

bool Engine::Initialize(int width = 1280, int height = 720, bool fullscreen = false)
{
	m_fullscreen = fullscreen;

	//Create and initialize the input object. Object used to handle input of the user
	m_input = new Input();

	InitializeWindows(width, height);

	//Create and initialize the application object. Object used to handle graphics for application
	m_application = new Application;

	return m_application->Initialize(width, height, m_fullscreen, m_hwnd, m_windowApplicationName);
}

void Engine::Shutdown()
{
	//Release application object
	if (m_application)
	{
		m_application->Shutdown();
		delete m_application;
		m_application = nullptr;
	}

	//Release input object
	if (m_input)
	{
		delete m_input;
		m_input = nullptr;
	}

	ShutdownWindows();
}

void Engine::Run()
{
	MSG message;

	//Init the message struct
	ZeroMemory(&message, sizeof(MSG));

	//Loop until there is a quit message from the window or user
	while (true)
	{
		//Handle window messages
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		//If window signals to end the application then exit
		if (message.message == WM_QUIT)
		{
			break;
		}

		//frame processing
		if (!Frame())
		{
			break;
		}			
	}
}

LRESULT Engine::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
		{
			DestroyWindow(hwnd);
		}
		m_input->KeyDown(static_cast<unsigned int>(wparam));
		break;
	case WM_KEYUP:
		m_input->KeyUp(static_cast<unsigned int>(wparam));
		break;
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}

	return 0;
}

bool Engine::Frame()
{
	if (m_input->IsKeyDown(VK_ESCAPE)) return false;

	if (!m_application->Frame()) return false;
	
	return true;
}

void Engine::InitializeWindows(int& width, int& height)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//get an external pointer to this object
	ApplicationHandle = this;

	//get instance of this application
	m_hinstance = GetModuleHandle(nullptr);

	//Setup windows class with default settings
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_classApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//Register the window class
	RegisterClassEx(&wc);

	//setup the screen settings depending on whether its running in full screen or windowed mode
	if (m_fullscreen)
	{
		//Determine the resolution of the clients desktop scren
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
		// If full screen set the screen to maxiumum size of the users desktop and 32bit
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)width;
		dmScreenSettings.dmPelsHeight = (unsigned long)height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Change the display settings to fullscreen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//set the position of the window to the top left corner
		posX = posY = 0;
	}
	else
	{
		//if windowed then set to resolution
		posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	}
	
	//create the window with the screen settings and set handle to it
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_classApplicationName, m_windowApplicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX | WS_OVERLAPPEDWINDOW,
		posX, posY, width, height, nullptr, nullptr, m_hinstance, nullptr);

	if (!m_hwnd)
	{

		Logger::Log(LogLevel::ERROR, "Failed to create the window.");
		MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
		return;
	}
	
	//Bring up the window on the screen and set it to focus
	SetWindowText(m_hwnd, m_windowApplicationName);
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//Hide the mouse cursor
	//ShowCursor(false);
}

void Engine::ShutdownWindows()
{
	//show the mouse cursor
	//ShowCursor(true);

	//Fix the display settings if leaving full screen mode
	if (m_fullscreen)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	//remove the window
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	//remove application class
	UnregisterClass(m_classApplicationName, m_hinstance);
	m_hinstance = nullptr;

	//release the pointer to the class
	m_application = nullptr;
}

void Engine::CreateConsole()
{
	AllocConsole();

	FILE* file;
	freopen_s(&file, "CONOUT$", "w", stdout);
	freopen_s(&file, "CONOUT$", "w", stderr);
	freopen_s(&file, "CONIN$", "r", stdin);

	std::ios::sync_with_stdio();

	Logger::Log(LogLevel::INFO, "Console has been created successfully.");
}

LRESULT WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		// Check if the window is being closed.
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
		// All other messages pass to the message handler in the system class.
	default:
		return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
	}
}