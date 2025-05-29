#include "ColorShader.hpp"

ColorShader::ColorShader() : m_vertexShader(nullptr), m_pixelShader(nullptr), m_inputLayout(nullptr), m_matrixBuffer(nullptr)
{
}

ColorShader::ColorShader(const ColorShader& colorShader)
{
}

ColorShader::~ColorShader()
{
}

bool ColorShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	wchar_t vsFilename[128];
	wchar_t psFilename[128];

	//Set filename of the VertexShader
	if (!wcscpy_s(vsFilename, 128, L"../Shader/Color.vs"))
	{
		Logger::Log(ERROR, "Failed to find file for vertexshader!");
		return false;
	}

	//filename of pixelshader
	if (!wcscpy_s(psFilename, 128, L"../Shader/PixelColor.ps"))
	{
		Logger::Log(ERROR, "Failed to find file for pixelshader!");
		return false;
	}

	//initialize the shaders
	if (!InitializeShader(device, hwnd, vsFilename, psFilename))
	{
		Logger::Log(ERROR, "Failed to initialize Shaders!");
		return false;
	}

	Logger::Log(INFO, "Initialized all shaders.");
	return true;
}

void ColorShader::Shutdown()
{
	//Shutdown the shaders as well as the related objects
	ShutdownShader();

	Logger::Log(INFO, "All shaders and related objects have been shutdown");
}

bool ColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
{
	//set shader parameters for rendering
	if (!SetShaderParameters(deviceContext, world, view, projection))
	{
		Logger::Log(ERROR, "Failed to set shader parameters for rendering");
		return false;
	}

	//render prepared buffer with shader
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShader, WCHAR* pixelShader)
{
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	D3D11_BUFFER_DESC matrixBufferDesc;

	if (FAILED(D3DCompileFromFile(vertexShader, nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
	{
		if (errorMessage)
		{
			const char* e = static_cast<char*>(errorMessage->GetBufferPointer());
			std::string error = e;
			Logger::Log(ERROR, "Failed to compile shader Vertex Shader" + error);
		}
		else
		{
			Logger::Log(ERROR, "Missing shader file");
		}
		return false;
	}

	if (FAILED(D3DCompileFromFile(pixelShader, nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
	{
		if (errorMessage)
		{
			const char* e = static_cast<char*>(errorMessage->GetBufferPointer());
			std::string error = e;
			Logger::Log(ERROR, "Failed to compile shader Pixel Shader" + error);
		}
		else
		{
			Logger::Log(ERROR, "Missing shader file");
		}
		return false;
	}

	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader)))
	{
		Logger::Log(ERROR, "Failed to create Vertex Shader");
		return false;
	}

	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader)))
	{
		Logger::Log(ERROR, "Failed to create Pixel Shader");
		return false;
	}

	//create the vertex input layout description
	//need to match Vertextyp struct in Model and shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	if (FAILED(device->CreateInputLayout(polygonLayout, sizeof(polygonLayout) / sizeof(polygonLayout[0]), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_inputLayout)))
	{
		Logger::Log(ERROR, "Failed to create Input layout");
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	//setup the description of the dynamic matrix constant buffer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer)))
	{
		Logger::Log(ERROR, "Failed to create buffer");
		return false;
	}

	return true;
}

void ColorShader::ShutdownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_inputLayout)
	{
		m_inputLayout->Release();
		m_inputLayout = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}
}

bool ColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* dataPtr;
	unsigned int bufferNumber;

	//Transpose the matrices to prepare them for the shader
	world = DirectX::XMMatrixTranspose(world);
	view = DirectX::XMMatrixTranspose(view);
	projection = DirectX::XMMatrixTranspose(projection);

	//Lock the constant buffer so it can be written to
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		Logger::Log(ERROR, "Failed to map to buffer");
		return false;
	}

	//Get a ptr to the data in the constant buffer
	dataPtr = static_cast<MatrixBuffer*>(mappedResource.pData);

	//Copy the matrices into the constant buffer
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;

	//Unlock the constant buffer
	deviceContext->Unmap(m_matrixBuffer, 0);

	//set position of the constant buffer in the vertex shader
	bufferNumber = 0;

	//finally set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//Set the vertex input layout
	deviceContext->IASetInputLayout(m_inputLayout);

	//Set the vertex and pixel shaders that will be used to render the triangle
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	//render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
