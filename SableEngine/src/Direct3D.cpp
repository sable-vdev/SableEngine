#include "Direct3D.hpp"
#include <iostream>
#include <fstream>

#ifdef ERROR
#undef ERROR
#endif
Direct3D::Direct3D() : m_swapChain(nullptr), m_device(nullptr), m_deviceContext(nullptr), m_renderTargetView(nullptr),
    m_depthStencilBuffer(nullptr), m_depthStencilState(nullptr), m_depthStencilView(nullptr), m_rasterState(nullptr)
{
}

Direct3D::Direct3D(const Direct3D& other) : m_swapChain(nullptr), m_device(nullptr), m_deviceContext(nullptr), m_renderTargetView(nullptr),
    m_depthStencilBuffer(nullptr), m_depthStencilState(nullptr), m_depthStencilView(nullptr), m_rasterState(nullptr)
{
}

Direct3D::~Direct3D()
{

}

bool Direct3D::Initialize(int& width, int& height, bool& vsync, HWND hwnd, bool& fullscreen, float screenDepth, float screenNear)
{
    IDXGIFactory* factory = 0;
    IDXGIAdapter* adapter{};
    IDXGIOutput* adapterOutput{};
    unsigned int numModes = 0, numerator = 0, denominator = 0;
    size_t stringLength = 128;
    DXGI_MODE_DESC* displayModeList{};
    DXGI_ADAPTER_DESC adapterDesc{};
    DXGI_SWAP_CHAIN_DESC swapChainDesc{};
    //set the feature level to directx 11 or directx 10, if 11 isnt available
    const D3D_FEATURE_LEVEL featureLevel[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};
    ID3D11Texture2D* backBufferPtr{};
    D3D11_TEXTURE2D_DESC depthBufferDesc{};
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
    D3D11_RASTERIZER_DESC rasterDesc{};
    float fieldOfView = 0.0f, screenAspect = 0.0f;

    //Store vsync setting
    m_vsyncEnabled = vsync;
    
    //Create a DirectX graphics interface factory
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory))) return false;

    //use the factory to create an adapter for the primary video card
    if (FAILED(factory->EnumAdapters(0, &adapter))) return false;

    //Enumerate the primary adapter output(monitor)
    if (FAILED(adapter->EnumOutputs(0, &adapterOutput))) return false;

    //Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor)
    if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr))) return false;

    //create a list to hold all the possible display modes for this monitor/video card combination
    if (!(displayModeList = new DXGI_MODE_DESC[numModes])) return false;

    //fill display mode list structures
    if(FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList))) return false;

    //go through all the display modes and find the one that matches the screen width and height
    // when a match is found store the numerator and denominator of the refresh rate of that monitor
    for (unsigned int i = 0; i < numModes; i++)
    {
        if (displayModeList[i].Width == (unsigned int)width && displayModeList[i].Height == (unsigned int)height)
        {
            numerator = displayModeList[i].RefreshRate.Numerator;
            denominator = displayModeList[i].RefreshRate.Denominator;
        }
    }

    //get the adapter (video card) description
    if (FAILED(adapter->GetDesc(&adapterDesc))) return false;

    //store dedicated video card memory in megabytes
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    //Convert the name of the video card to a character array and store it
    if(wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0) return false;

    //Release the display mode list
    delete[] displayModeList;
    displayModeList = nullptr;

    //Release the adapter output
    adapterOutput->Release();
    adapterOutput = nullptr;

    //Release the adapter
    adapter->Release();
    adapter = nullptr;

    //Release the factory
    factory->Release();
    factory = nullptr;

    //Initialize the swap chain description
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    //set to a single back buffer
    swapChainDesc.BufferCount = 1;

    //Set the width and height of the back buffer
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;

    //set regular 32-bit surface for the back buffer
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    //set refresh rate of the back buffer
    if (fullscreen)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    //set the usage of the back buffer
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    //Set the handle for the window to render to
    swapChainDesc.OutputWindow = hwnd;

    //turn multisampling off
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    //set to full screen or windows
    if (fullscreen)
    {
        swapChainDesc.Windowed = false;
    }
    else swapChainDesc.Windowed = true;

    //set the scan line ordering and scaling to unspecified
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    //Discard the back buffer contents after presenting
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    //dont set the advanced flags
    swapChainDesc.Flags = 0;

    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifndef NDEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !NDEBUG


    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevel, sizeof(featureLevel) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &swapChainDesc,
        &m_swapChain, &m_device, nullptr, &m_deviceContext)))
    {
        Logger::Log(ERROR, "Failed to create device and swapchain on a discrete gpu");
        if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_REFERENCE, nullptr, 0, featureLevel, sizeof(featureLevel) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &swapChainDesc,
            &m_swapChain, &m_device, nullptr, &m_deviceContext)))
        {
            Logger::Log(ERROR, "Failed to create device and swapchain on a integrated gpu");
            return false;
        }
    }
    //get the pointer to the back buffer
    if(FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr))) return false;

    //create the render target view with the back buffer pointer
    if (FAILED(m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView))) return false;

    //release pointer to the back buffer as we no longer need it
    backBufferPtr->Release();
    backBufferPtr = nullptr;

    //Initialize the description of the depth buffer
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    //set up the description of the depth buffer
    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    //Create the texture for the depth buffer using the filled oput description
    if (FAILED(m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer))) return false;

    //Initialize the description of the stencil state
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    //set up the description of the stencil state
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    //Stencil operations if pixel is front facing
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back facing
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    //create the depth stencil state
    if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState))) return false;

    // set the depth stencil state
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

    //Initialize the depth stencil view
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    //Set up the depth stencil view description
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    if (FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView))) return false;

    //Bind the render target view and depth stencil buffer to the output render pipeline
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    //Setup the raster description which will determine how and what polygons will be drawn
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    //Create the rasterizer state from the description
    if(FAILED(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState))) return false;

    //now set the rasterizer state
    m_deviceContext->RSSetState(m_rasterState);

    //Setup the viewport for rendering
    m_viewport.Width = (float)width;
    m_viewport.Height = (float)height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    //create the viewport
    m_deviceContext->RSSetViewports(1, &m_viewport);

    //setup the projection matrix
    fieldOfView = 3.141592654f / 4.0f;
    screenAspect = (float)width / (float) height;

    //create the projection matrix for 3d rendering
    projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

    //initialize the world matrix to the identity matrix
    worldMatrix = DirectX::XMMatrixIdentity();

    //create an orthographic projection matrix for 2d rendering
    orthoMatrix = DirectX::XMMatrixOrthographicLH((float)width, (float)height, screenNear, screenDepth);
    Logger::Log(LogLevel::INFO, GetVideoCardInfo());
    return true;
}

void Direct3D::Shutdown()
{
    if (m_swapChain) m_swapChain->SetFullscreenState(false, nullptr);

    if (m_rasterState)
    {
        m_rasterState->Release();
        m_rasterState = nullptr;
    }

    if (m_depthStencilView)
    {
        m_depthStencilView->Release();
        m_depthStencilView = nullptr;
    }

    if (m_depthStencilBuffer)
    {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = nullptr;
    }
    if (m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }

    if (m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = nullptr;
    }

    if (m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }

    if (m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }

}

void Direct3D::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4] = { red, green, blue, alpha };

    //Clear the back buffer
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

    //Clear the depth buffer
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Direct3D::EndScene()
{
    //present the back buffer to the screen since rendering is complete
    if (m_vsyncEnabled)
    {
        m_swapChain->Present(1, 0);
    }
    else m_swapChain->Present(0, 0);
}

ID3D11Device* Direct3D::GetDevice()
{
    return m_device;
}

ID3D11DeviceContext* Direct3D::GetDeviceContext()
{
    return m_deviceContext;
}

std::string Direct3D::GetVideoCardInfo()
{
    std::string info;
    for (int i = 0; i < 128; i++)
    {
        if (m_videoCardDescription[i] == '\0') break;
        info += m_videoCardDescription[i];
    }
    info = "Using: " + info + " " + std::to_string(m_videoCardMemory) + " MB";
    return info;
}

void Direct3D::SetBackBufferRenderTarget()
{
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void Direct3D::ResetViewport()
{
    m_deviceContext->RSSetViewports(1, &m_viewport);
}
