#include "Application.hpp"

Application::Application() : m_d3dClass(nullptr), m_hwnd(nullptr), m_windowApplicationName(nullptr),
    m_deltaTime(0.0f), m_timer(0.0f)
{
    QueryPerformanceCounter(&m_lastTime);
    QueryPerformanceFrequency(&m_frequency);
}

Application::Application(const Application& other)
{
}

bool Application::Initialize(int& width, int& height, bool& fullscreen, HWND hwnd, LPCWSTR applicationName)
{
    m_d3dClass = new Direct3D;
    m_hwnd = hwnd;
    m_windowApplicationName = applicationName;
    return m_d3dClass->Initialize(width, height, m_vsyncEnabled, hwnd, fullscreen, SCREEN_DEPTH, SCREEN_NEAR);
}

void Application::Shutdown()
{
    //Release Direct3D object
    if (m_d3dClass)
    {
        m_d3dClass->Shutdown();
        delete m_d3dClass;
        m_d3dClass = nullptr;
    }
}

bool Application::Frame()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    m_deltaTime = static_cast<float>(currentTime.QuadPart - m_lastTime.QuadPart) / m_frequency.QuadPart;

    m_timer += m_deltaTime;

    if (m_timer >= 1.0f)
    {
        m_timer = 0.0f;
        
        SetFpsTitle();
        m_fps = 0;
    }
    m_lastTime = currentTime;
    m_fps++;
    //Render grahpics scene
    return Render();
}

bool Application::Render()
{
    //Clear the buffers to begin the scene
    m_d3dClass->BeginScene(0.75f, 0.75f, 0.5f, 1.0f);

    //Present the rendered scene to the screen
    m_d3dClass->EndScene();

    return true;
}

void Application::SetFpsTitle() 
{
    std::wstringstream wss{};
    wss << m_windowApplicationName << " [" << m_fps << " (" << 1000.0 / m_fps << "ms)]";

    fpsString = wss.str();
    /*
    RECT rect{};
    GetWindowRect(m_hwnd, &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    Logger::Log(LogLevel::INFO, "Width: " + std::to_string(width) + " Height:" + std::to_string(height));
    */
    SetWindowText(m_hwnd, fpsString.c_str());
}