#pragma once
#include "Logger.hpp"
#include <d3dcompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <stdexcept>
#include <fstream>
#ifdef ERROR
#undef ERROR
#endif
class ColorShader
{
private:
	
	struct MatrixBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_inputLayout;
	ID3D11Buffer* m_matrixBuffer;

public:
	ColorShader();
	ColorShader(const ColorShader& colorShader);
	~ColorShader();
	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShader, WCHAR* pixelShader);
	void ShutdownShader();
	//void OutputShaderErrorMessage(ID3D10Blob* blob, HWND hwnd, WCHAR* wchar);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
};

