#include "depthShader.h"
#include <fstream>


DepthShader::DepthShader():
	m_vertexShader(nullptr),m_pixelShader(nullptr),m_layout(nullptr),m_matrixBuffer(nullptr)
{
}
DepthShader::~DepthShader()
{
	if (m_matrixBuffer)	m_matrixBuffer->Release();
	if (m_layout) m_layout->Release();
	if (m_vertexShader) m_vertexShader->Release();
	if (m_pixelShader) m_pixelShader->Release();
}

bool DepthShader::Init(ID3D11Device *dev, HWND hwnd)
{
	bool  result = true;
	
	result = InitShader(dev,hwnd,"Shaders/vs_depth.hlsl", "Shaders/ps_depth.hlsl");
	if (!result) return false;
	return result;
}

bool DepthShader::Render(ID3D11DeviceContext * devCon, int indexCount, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix)
{
	bool result = true;
	result = SetShaderParameters(devCon,worldMatrix,viewMatrix,projectionMatrix);
	if (!result) return false;
	RenderShader(devCon,indexCount);
	return result;
}

bool DepthShader::InitShader(ID3D11Device * device, HWND hwnd, CHAR *vsFilename, CHAR *psFilename)
{
	using namespace DirectX;
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;

	D3D11_BUFFER_DESC matrixBufferDesc;

	errorMessage=nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	wchar_t temp[100];
	swprintf(temp, 100, L"%hs", vsFilename);
	result = D3DCompileFromFile(temp, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if(errorMessage)OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		return false;
	}

	swprintf(temp, 100, L"%hs", psFilename);
	result = D3DCompileFromFile(temp, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		return false;
	}
	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) return false;

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) return false;

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	result = device->CreateInputLayout(polygonLayout,numElements,vertexShaderBuffer->GetBufferPointer(), 
		vertexShaderBuffer->GetBufferSize(),&m_layout);
	if (FAILED(result)) return false;

	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) return false;

	return true;
}

void DepthShader::OutputShaderErrorMessage(ID3D10Blob *errorMessage, HWND hwnd, CHAR *shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();
	fout.open("shader-error.txt");
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();
	errorMessage->Release();

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

}

bool DepthShader::SetShaderParameters(ID3D11DeviceContext *devCon, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix)
{
	using namespace DirectX;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedRessource;
	unsigned int bufferNumber;
	MatrixBufferType* datePtr;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// locking constant buffer, to write
	result = devCon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD,0, &mappedRessource);
	if (FAILED(result)) return false;
	datePtr = (MatrixBufferType*)mappedRessource.pData;
	datePtr->world = worldMatrix;
	datePtr->view = viewMatrix;
	datePtr->projection = projectionMatrix;

	devCon->Unmap(m_matrixBuffer, 0);
	bufferNumber = 0;
	devCon->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	return true;
}

void DepthShader::RenderShader(ID3D11DeviceContext * devCon, int indexCount)
{
	devCon->IASetInputLayout(m_layout);
	devCon->VSSetShader(m_vertexShader, NULL, 0);
	devCon->PSSetShader(m_pixelShader, NULL, 0);
	devCon->DrawIndexed(indexCount, 0, 0);
	devCon->GSSetShader(nullptr, nullptr, 0);
	devCon->VSSetShader(nullptr, nullptr, 0);
	devCon->PSSetShader(nullptr, nullptr, 0);
}
