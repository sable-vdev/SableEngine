#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Logger.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <stdexcept>

class Direct3D
{
private:
	bool m_vsyncEnabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	D3D11_VIEWPORT m_viewport;
public:
	DirectX::XMMATRIX projectionMatrix;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX orthoMatrix;
public:
	Direct3D();
	Direct3D(const Direct3D& other);
	~Direct3D();
	bool Initialize(int& width, int& height, bool& vsync, HWND hwnd, bool& fullscreen, float screenDepth, float screenNear);
	void Shutdown();
	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	std::string GetVideoCardInfo();

	void SetBackBufferRenderTarget();
	void ResetViewport();
};

