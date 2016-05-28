#include "TerrainShader.h"



TerrainShader::TerrainShader(): m_vertexShader(nullptr)
							  , m_hullShader			(nullptr)
							  , m_domainShader		(nullptr)
							  , m_pixelShader		(nullptr)
							  , m_layout				(nullptr)
							  , m_samplerText		(nullptr)
							  , m_samplerDisp		(nullptr)
							  , m_matrixBuffer		(nullptr)
							  , m_tesseletionBuffer	(nullptr)
{
}


TerrainShader::~TerrainShader()
{
	if (m_vertexShader
		&&	m_hullShader
		&&	m_domainShader
		&&	m_pixelShader
		&&	m_layout
		&&	m_samplerText
		&&	m_samplerDisp
		&&	m_matrixBuffer
		&&	m_tesseletionBuffer
		)
	{
		m_vertexShader->Release();
		m_hullShader->Release();
		m_domainShader->Release();
		m_pixelShader->Release();
		m_layout->Release();
		m_samplerText->Release();
		m_samplerDisp->Release();
		m_matrixBuffer->Release();
		m_tesseletionBuffer->Release();
	}
}

bool TerrainShader::Init(ID3D11Device *device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitShader(device, hwnd, "Shaders/vs_terrain.hlsl", "Shaders/ds_terrain.hlsl", "Shaders/hs_terrain.hlsl", "Shaders/ps_terrain.hlsl");
	if (!result) return false;
	
	return true;
}

bool TerrainShader::Render(ID3D11DeviceContext * devCon, ID3D11Device * device, int indexCount, Matrices sceneInfo, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * heightmap)
{
	SetShaderParameters(devCon, device, sceneInfo, texture, heightmap);
	RenderShader(devCon, indexCount);
}

bool TerrainShader::InitShader(ID3D11Device * device, HWND hwnd, CHAR *vsFilename, CHAR *hsFilename, CHAR *dsFilename, CHAR *psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	ID3D10Blob* hullShaderBuffer;
	ID3D10Blob* domainShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;

	D3D11_BUFFER_DESC matrixBufferDesc;

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

	wchar_t temp[100];
	swprintf(temp, 100, L"%hs", hsFilename);
	// Compile the vertex shader code.
	result = D3DCompileFromFile(temp, NULL, NULL, "main", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&hullShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, hsFilename);

		return false;
	}

	wchar_t temp[100];
	swprintf(temp, 100, L"%hs", dsFilename);
	// Compile the vertex shader code.
	result = D3DCompileFromFile(temp, NULL, NULL, "main", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&domainShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, dsFilename);

		return false;
	}

	wchar_t temp[100];
	swprintf(temp, 100, L"%hs", psFilename);
	// Compile the vertex shader code.
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
	result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader);
	if (FAILED(result)) return false;
	
	// Create the vertex shader from the buffer.
	result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader);
	if (FAILED(result)) return false;

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) return false;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) return false;

	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();
	hullShaderBuffer->Release();
	domainShaderBuffer->Release();
	errorMessage->Release();

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_samplerDisp);
	if (FAILED(result))	return false;

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_samplerText);
	if (FAILED(result))	return false;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) return false;

}

void TerrainShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, CHAR* shaderFilename)
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

	return;
}

bool TerrainShader::SetShaderParameters(ID3D11DeviceContext * devCon, ID3D11Device * device, Matrices sceneInfo, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * heightmap)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	TesselationBufferType* dataPtr2;

	sceneInfo.worldMatrix = XMMatrixTranspose(sceneInfo.worldMatrix);
	sceneInfo.viewMatrix = XMMatrixTranspose(sceneInfo.viewMatrix);
	sceneInfo.projectionMatrix = XMMatrixTranspose(sceneInfo.projectionMatrix);


	result = devCon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) return false;

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = sceneInfo.worldMatrix;
	dataPtr->view = sceneInfo.viewMatrix;
	dataPtr->projection = sceneInfo.projectionMatrix;

	// Unlock the constant buffer.
	devCon->Unmap(m_matrixBuffer, 0);


	result = devCon->Map(m_tesseletionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) return false;

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (TesselationBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr2->edgefactors = DirectX::XMFLOAT4(m_uniformTesseletionFactor, m_uniformTesseletionFactor, m_uniformTesseletionFactor, m_uniformTesseletionFactor);
	dataPtr2->innerfactors = DirectX::XMFLOAT2(m_uniformTesseletionFactor, m_uniformTesseletionFactor);

	// Unlock the constant buffer.
	devCon->Unmap(m_tesseletionBuffer, 0);


	//Set Ressources

	devCon->DSSetConstantBuffers(0, 1, &m_matrixBuffer);
	devCon->HSSetConstantBuffers(0, 1, &m_tesseletionBuffer);

	devCon->DSSetShaderResources(0, 1, &heightmap);
	devCon->PSSetShaderResources(0, 1, &texture);

	devCon->DSSetSamplers(0, 1, &m_samplerDisp);
	devCon->PSSetSamplers(0, 1, &m_samplerText);

	return true;
}

void TerrainShader::RenderShader(ID3D11DeviceContext *devCon, int indexCount)
{
	devCon->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	devCon->VSSetShader(m_vertexShader, NULL, 0);
	devCon->HSSetShader(m_hullShader, NULL, 0);
	devCon->DSSetShader(m_domainShader, NULL, 0);
	devCon->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	devCon->DrawIndexed(indexCount, 0, 0);
	devCon->GSSetShader(nullptr, nullptr, 0);
	devCon->VSSetShader(nullptr, nullptr, 0);
	devCon->PSSetShader(nullptr, nullptr, 0);
	devCon->DSSetShader(nullptr, nullptr, 0);
	devCon->HSSetShader(nullptr, nullptr, 0);
}
