#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Logger.hpp"

class Model
{
private:
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	ID3D11Buffer* m_vertexBuffer,* m_indexBuffer;
	int m_vertexCount, m_indexCount;

public:
	Model();
	Model(const Model& other);
	~Model();
	bool Initialize(ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount() const;
private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);
};

