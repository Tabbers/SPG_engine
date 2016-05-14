#include "lineShader.h"
#include <string>


LineShader::LineShader(): m_vertexShader(nullptr), m_pixelShader(nullptr), m_layout(nullptr)
{
}


LineShader::~LineShader()
{
	m_layout->Release();
	m_pixelShader->Release();
	m_vertexShader->Release();
}

bool LineShader::SetShaderParameters(ID3D11DeviceContext* devcon, ID3D11Device* device, Matrices sceneInfo)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_SAMPLER_DESC samplerDesc;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	sceneInfo.worldMatrix = XMMatrixTranspose(sceneInfo.worldMatrix);
	sceneInfo.viewMatrix = XMMatrixTranspose(sceneInfo.viewMatrix);
	sceneInfo.projectionMatrix = XMMatrixTranspose(sceneInfo.projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = devcon->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) return false;

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = sceneInfo.worldMatrix;
	dataPtr->view = sceneInfo.viewMatrix;
	dataPtr->projection = sceneInfo.projectionMatrix;

	// Unlock the constant buffer.
	devcon->Unmap(matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);
}

bool LineShader::Init(ID3D11Device *device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitShader(device, hwnd, "Shaders/vs_line.hlsl", "Shaders/ps_line.hlsl");
	if (!result) return false;

	return true;
}

bool LineShader::Render(ID3D11DeviceContext* devCon, ID3D11Device* device, int indexCount,  Matrices sceneInfo)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(devCon, device, sceneInfo);
	if (!result)	return false;

	// Now render the prepared buffers with the shader.
	RenderShader(devCon, indexCount);

	return true;
}

bool LineShader::InitShader(ID3D11Device *device, HWND hwnd, CHAR *vsFilename, CHAR *psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc2;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	wchar_t temp[100];
	swprintf(temp, 100, L"%hs", vsFilename);

	// Compile the vertex shader code.
	result = D3DCompileFromFile(temp, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);

		return false;
	}

	swprintf(temp, 100, L"%hs", psFilename);
	// Compile the pixel shader code.
	result = D3DCompileFromFile(temp, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, psFilename);

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

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) return false;

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(result)) return false;

	return true;
}

void LineShader::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, CHAR *shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	std::ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

}

void LineShader::RenderShader(ID3D11DeviceContext * devCon, int indexCount)
{
	// Set the vertex input layout.
	devCon->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	devCon->VSSetShader(m_vertexShader, NULL, 0);
	devCon->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	devCon->DrawIndexed(indexCount, 0, 0);
	devCon->GSSetShader(nullptr, nullptr, 0);
	devCon->VSSetShader(nullptr, nullptr, 0);
	devCon->PSSetShader(nullptr, nullptr, 0);
}
