#pragma once
#include "Application.hpp"
#include "Input.hpp"

class Engine
{
private:
	bool m_fullscreen = false;
	LPCWSTR m_classApplicationName = L"SableEngineWindowClass";
	LPCWSTR m_windowApplicationName = L"Sable Render Engine";
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	Application* m_application;
	Input* m_input;
public:
	Engine();
	Engine(int width, int height, bool fullscreen);
	Engine(const Engine& other);
	~Engine();
	void Run();
	void Shutdown();
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
private:
	bool Initialize(int width, int height, bool fullscreen);
	bool Frame();
	void InitializeWindows(int& width, int& height);
	void ShutdownWindows();
	void CreateConsole();
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

static Engine* ApplicationHandle = 0;