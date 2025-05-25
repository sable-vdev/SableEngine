#pragma once
#include "D3DClass.hpp"

class Application
{
private:
	LPCWSTR m_windowApplicationName;
	const float SCREEN_NEAR = 0.3f;
	const float SCREEN_DEPTH = 1000.0f;
	bool m_vsyncEnabled = false;
	HWND m_hwnd;
	D3DClass* m_d3dClass;
	int m_frameCount = 0;
	int m_fps = 0;
	LARGE_INTEGER m_startTime;
	LARGE_INTEGER m_frequency;
public:
	std::wstring fpsString;
public:
	Application();
	Application(const Application& other);
	bool Initialize(int& width, int& height, bool& fullscreen, HWND hwnd, LPCWSTR applicationName);
	void Shutdown();
	bool Frame();
private:
	bool Render();
	void SetFpsTitle();
};