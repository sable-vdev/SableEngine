#include "Application.hpp"

Application::Application() : m_d3dClass(nullptr)
{
    QueryPerformanceCounter(&m_startTime);
    QueryPerformanceFrequency(&m_frequency);

}

Application::Application(const Application& other) : m_d3dClass(nullptr)
{
}

bool Application::Initialize(int& width, int& height, bool& fullscreen, HWND hwnd, LPCWSTR applicationName)
{
    m_d3dClass = new D3DClass;
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
    m_frameCount++;

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    float elapsedTime = static_cast<float>(currentTime.QuadPart - m_startTime.QuadPart) / m_frequency.QuadPart;

    if (elapsedTime >= 1.0f)
    {
        m_fps = m_frameCount;
        m_frameCount = 0;
        m_startTime = currentTime;

        std::wstringstream wss{};
        wss << m_windowApplicationName;

        wss << " [" << m_fps << " (" << 1000.0/m_fps << "ms)]";

        fpsString = wss.str();
        SetFpsTitle();

        RECT rect{};
        GetWindowRect(m_hwnd, &rect);

        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        Logger::Log(LogLevel::INFO,"Width: " + std::to_string(width) + " Height:" + std::to_string(height));
    }

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

void Application::SetFpsTitle() const
{
    SetWindowText(m_hwnd, fpsString.c_str());
}
