#pragma once
#include "Direct3D.hpp"

class Application
{
private:
	const float SCREEN_NEAR = 0.3f;
	const float SCREEN_DEPTH = 1000.0f;
	bool m_vsyncEnabled = false;
	Direct3D* m_d3dClass;
	HWND m_hwnd;
	LPCWSTR m_windowApplicationName;
	int m_frameCount = 0;
	int m_fps = 0;
	LARGE_INTEGER m_lastTime;
	LARGE_INTEGER m_frequency;
	float m_timer = 0.0f;
	float m_deltaTime = 0.0f;
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