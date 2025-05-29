#include "Model.hpp"

#ifdef ERROR
#undef ERROR
#endif // ERROR

Model::Model() : m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_vertexCount(0), m_indexCount(0)
{
	Logger::Log(DEBUG, "Created model object");
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
	Logger::Log(DEBUG, "Destroyed model object");
}

bool Model::Initialize(ID3D11Device* device)
{
	//Initialize vertex and index buffer
	if (!InitializeBuffers(device))
	{
		Logger::Log(ERROR, "Failed to initialize buffers");
		return false;
	}
	return true;
}

void Model::Shutdown()
{
	//Shutdown both vertex and index buffer
	ShutdownBuffers();
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	//Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(deviceContext);
}

int Model::GetIndexCount() const
{
	return m_indexCount;
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices = 0;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//set the number of vertecies in the vertex array
	m_vertexCount = 3;

	//set the number of indices in the index array
	m_indexCount = 3;

	//create vertex array
	if (!(vertices = new VertexType[m_vertexCount]))
	{
		Logger::Log(ERROR, "Failed to create the vertex array");
		return false;
	}

	if (!(indices = new unsigned long[m_indexCount]))
	{
		Logger::Log(ERROR, "Failed to create the index array");
		return false;
	}

	vertices[0].position = DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[0].color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); 

	vertices[2].position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	return true;
}

void Model::ShutdownBuffers()
{
}

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
}
