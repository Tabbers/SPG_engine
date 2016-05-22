#include "colorshader.h"


ColorShader::ColorShader():
	vertexShader(nullptr), pixelShader(nullptr),layout(nullptr),matrixBuffer(nullptr)
{
}

ColorShader::~ColorShader()
{
	if (matrixBuffer)
	{
		matrixBuffer->Release();
	}
	if (layout)
	{
		layout->Release();
	}
	if (pixelShader)
	{
		pixelShader->Release();
	}
	if (vertexShader)
	{
		vertexShader->Release();
	}
}


bool ColorShader::Init(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
 	result = InitShader(device, hwnd, "Shaders/vs_color.hlsl", "Shaders/ps_color.hlsl");
	if(!result) return false;

	return true;
}


bool ColorShader::Render(ID3D11DeviceContext* deviceContext, ID3D11Device* device, int indexCount, Matrices sceneInfo, LightData lightInfo,
	ID3D11ShaderResourceView* depthMapTexture, ID3D11ShaderResourceView* texture,ID3D11ShaderResourceView* normalMap, ID3D11ShaderResourceView* displacemntMap)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext,device,sceneInfo,lightInfo,depthMapTexture,texture,normalMap,displacemntMap);
	if(!result)	return false;

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool ColorShader::InitShader(ID3D11Device* device, HWND hwnd, CHAR* vsFilename, CHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc2;
	D3D11_BUFFER_DESC cameraBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	wchar_t temp[100];
	swprintf(temp, 100, L"%hs", vsFilename);
    // Compile the vertex shader code.
	result = D3DCompileFromFile(temp, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
								&vertexShaderBuffer, &errorMessage);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);

		return false;
	}

	swprintf(temp, 100, L"%hs", psFilename);
    // Compile the pixel shader code.
	result = D3DCompileFromFile(temp, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
								&pixelShaderBuffer, &errorMessage);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage) OutputShaderErrorMessage(errorMessage, hwnd, psFilename);

		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if(FAILED(result)) return false;

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if(FAILED(result)) return false; 

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
									   vertexShaderBuffer->GetBufferSize(), &layout);
	if(FAILED(result)) return false;

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;


	m_sampleFilter = static_cast<D3D11_FILTER>(m_filtervalues[m_filter]);
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))	return false;

	// Create a texture sampler state description.
	samplerDesc.Filter = m_sampleFilter;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateText);
	if (FAILED(result))	return false;

	// Create a texture sampler state description.
	samplerDesc.Filter = m_sampleFilter;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateNormal);
	if (FAILED(result))	return false;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if(FAILED(result)) return false;

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	if (FAILED(result)) return false;

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
	lightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc2.MiscFlags = 0;
	lightBufferDesc2.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc2, NULL, &lightBuffer2);
	if (FAILED(result)) return false;

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(LightBufferType2);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);
	if (FAILED(result)) return false;


	return true;
}

void ColorShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, CHAR* shaderFilename)
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
	for(i=0; i<bufferSize; i++)
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


bool ColorShader::SetShaderParameters(ID3D11DeviceContext* devcon, ID3D11Device* device, Matrices sceneInfo, LightData lightInfo,
	ID3D11ShaderResourceView* depthMapTexture, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalMap, ID3D11ShaderResourceView* displacementMap)
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_SAMPLER_DESC samplerDesc;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	LightBufferType2* dataPtr3;
	CameraBufferType* dataPtr4;
	unsigned int bufferNumber;

	m_sampleFilter = static_cast<D3D11_FILTER>(m_filtervalues[m_filter]);

	// Create a texture sampler state description.
	samplerDesc.Filter = m_sampleFilter;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = m_mipmap;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateText);
	if (FAILED(result))	return false;

	// Create a texture sampler state description.
	samplerDesc.Filter = m_sampleFilter;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateNormal);
	if (FAILED(result))	return false;


	// Transpose the matrices to prepare them for the shader.
	sceneInfo.worldMatrix = XMMatrixTranspose(sceneInfo.worldMatrix);
	sceneInfo.viewMatrix = XMMatrixTranspose(sceneInfo.viewMatrix);
	sceneInfo.projectionMatrix = XMMatrixTranspose(sceneInfo.projectionMatrix);

	lightInfo.lightViewMatrix = XMMatrixTranspose(lightInfo.lightViewMatrix);
	lightInfo.lightProjectionMatrix = XMMatrixTranspose(lightInfo.lightProjectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = devcon->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)) return false;

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = sceneInfo.worldMatrix;
	dataPtr->view = sceneInfo.viewMatrix;
	dataPtr->projection = sceneInfo.projectionMatrix;

	dataPtr->lightView = lightInfo.lightViewMatrix;
	dataPtr->lightProjection = lightInfo.lightProjectionMatrix;

	// Unlock the constant buffer.
	devcon->Unmap(matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	result = devcon->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) return false;

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	dataPtr2->ambientColor      = lightInfo.ambientColor;
	dataPtr2->diffuseColor      = lightInfo.diffuseColor;
	dataPtr2->specularColor     = lightInfo.specularColor;
	dataPtr2->specularIntensity = lightInfo.specularIntensity;
	dataPtr2->lightDirection    = lightInfo.lightDirection;
	
	if (sceneInfo.DrawNormal)
	{
		dataPtr2->switches.x = 1.0f;
	}
	else
	{
		dataPtr2->switches.x = 0.0f;
	}
	
	if (sceneInfo.DrawSpec)
	{
		dataPtr2->switches.y = 1.0f;
	}
	else
	{
		dataPtr2->switches.y = 0.0f;
	}

	if (sceneInfo.DrawDisp)
	{
		dataPtr2->switches.z = 1.0f;
	}
	else
	{
		dataPtr2->switches.z = 0.0f;
	}

	if (sceneInfo.DrawHardShadows)
	{
		dataPtr2->switches.w = 1.0f;
	}
	else
	{
		dataPtr2->switches.w = 0.0f;
	}

	dataPtr2->stepsXRefiningStepsy.x = m_numberOfIterations;
	dataPtr2->stepsXRefiningStepsy.y = m_refinementIterations;
	// unlock light buffer
	devcon->Unmap(lightBuffer,0);
	//set the position of the light buffer in the pixelshader
	bufferNumber = 0;
	devcon->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer);
	
	result = devcon->Map(lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))	return false;
	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (LightBufferType2*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr3->lightPosition = lightInfo.lightPosition;
	dataPtr3->padding = 0.0f;

	// Unlock the constant buffer.
	devcon->Unmap(lightBuffer2, 0);

	result = devcon->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))	return false;
	// Get a pointer to the data in the constant buffer.
	dataPtr4 = (CameraBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr4->cameraPosition = sceneInfo.cameraPosition;
	dataPtr4->cameraDirection = lightInfo.cameraDirection;

	// Unlock the constant buffer.
	devcon->Unmap(cameraBuffer, 0);

	// Set the position of the light constant buffer in the vertex shader.
	bufferNumber = 1;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &lightBuffer2);
	// Finally set the light constant buffer in the pixel shader with the updated values.
	devcon->VSSetConstantBuffers(2, 1, &cameraBuffer);

	devcon->PSSetShaderResources(0, 1, &depthMapTexture);
	devcon->PSSetShaderResources(1, 1, &texture);
	devcon->PSSetShaderResources(2, 1, &normalMap);
	devcon->PSSetShaderResources(3, 1, &displacementMap);

	return true;
}


void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(1, 1, &m_sampleStateText);
	// Set the sampler state in te pixel shader.
	deviceContext->PSSetSamplers(2, 1, &m_sampleStateNormal);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(vertexShader, NULL, 0);
    deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->VSSetShader(nullptr, nullptr, 0);
	deviceContext->PSSetShader(nullptr, nullptr, 0);
	return;
}