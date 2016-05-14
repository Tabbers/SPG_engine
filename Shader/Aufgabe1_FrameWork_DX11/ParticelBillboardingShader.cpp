#include "ParticelBillboardingShader.h"

ParticelBillboardingShader::ParticelBillboardingShader()
{
}

ParticelBillboardingShader::~ParticelBillboardingShader()
{
}

HRESULT ParticelBillboardingShader::Init(ID3D11DeviceContext *devcon , ID3D11Device *device, const char* vsFilename, const char* gsFilename, const char* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* geometryShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[6];
	D3D11_SAMPLER_DESC samplerDesc;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC dataBufferDesc;

	unsigned int numElements;
	wchar_t temp[100];

	//Create Buffers

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	if (FAILED(result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer)))
		return S_FALSE;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	dataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	dataBufferDesc.ByteWidth = sizeof(DataType);
	dataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dataBufferDesc.MiscFlags = 0;
	dataBufferDesc.StructureByteStride = 0;

	if (FAILED(result = device->CreateBuffer(&dataBufferDesc, nullptr, &m_dataBuffer)))
		return S_FALSE;

	swprintf(temp, 100, L"%hs", vsFilename);
	//Compile Shaders
	if (FAILED(CompileShaderFromFile(temp, "main", "vs_5_0", &vertexShaderBuffer)))
	{
		return S_FALSE;
	}
	swprintf(temp, 100, L"%hs", gsFilename);
	if (FAILED(CompileShaderFromFile(temp, "main", "gs_5_0", &geometryShaderBuffer)))
	{
		return S_FALSE;
	}
	swprintf(temp, 100, L"%hs", psFilename);
	if (FAILED(CompileShaderFromFile(temp, "main", "ps_5_0", &pixelShaderBuffer)))
	{
		return S_FALSE;
	}

	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_VertexShader)))
		return S_FALSE;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "VELOCITY";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "SIZE";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "AGE";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "INTERVAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	polygonLayout[5].SemanticName = "TYPE";
	polygonLayout[5].SemanticIndex = 0;
	polygonLayout[5].Format = DXGI_FORMAT_R32_UINT;
	polygonLayout[5].InputSlot = 0;
	polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[5].InstanceDataStepRate = 0;

	device->CreateInputLayout(polygonLayout, 6u, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_VertexLayout);

	if (FAILED(device->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(),
		geometryShaderBuffer->GetBufferSize(), nullptr, &m_GeometryShader)))
		return S_FALSE;


	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_PixelShader)))
		return S_FALSE;

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
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

	if (FAILED(device->CreateSamplerState(&samplerDesc, &m_SamplerDiffuse)))
		return S_FALSE;


	vertexShaderBuffer->Release();
	geometryShaderBuffer->Release();
	pixelShaderBuffer->Release();

	return S_OK;
}

HRESULT ParticelBillboardingShader::Render(ID3D11DeviceContext *devcon, ID3D11Device *device, Matrices sceneInfo, ID3D11Buffer *vertexbuffer, ID3D11ShaderResourceView* diffuse, DirectX::XMVECTOR campos)
{
	UpdateBuffers(devcon, sceneInfo,campos);

	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(Particle);
	unsigned int offset = 0;
	UINT offset2 = 0u;

	devcon->IASetInputLayout(m_VertexLayout);
	devcon->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	devcon->VSSetShader(m_VertexShader, nullptr,0);
	// Set the vertex buffer to active in the input assembler so it can be rendered.

	devcon->GSSetShader(m_GeometryShader, nullptr, 0);
	//SetShaderParameters
	devcon->GSSetConstantBuffers(0, 1, &m_matrixBuffer);
	devcon->GSSetConstantBuffers(1, 1, &m_dataBuffer);

	devcon->PSSetShader(m_PixelShader, nullptr, 0);
	devcon->PSSetSamplers(0,1,&m_SamplerDiffuse);
	devcon->PSSetShaderResources(0,1,&diffuse);

	devcon->DrawAuto();

	devcon->GSSetShader(nullptr, nullptr, 0);
	devcon->VSSetShader(nullptr, nullptr, 0);
	devcon->PSSetShader(nullptr, nullptr, 0);
	return S_OK;
}
HRESULT ParticelBillboardingShader::CompileShaderFromFile(WCHAR * FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob ** Outblob)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debug output, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* errorBlob;
	//DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;


	hr = D3DCompileFromFile(FileName, NULL, NULL, EntryPoint, ShaderModel,
		dwShaderFlags, NULL, Outblob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob != NULL)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		if (errorBlob)
		{
			errorBlob->Release();
		}
		return hr;
	}

	if (errorBlob) errorBlob->Release();

	return S_OK;

}
HRESULT ParticelBillboardingShader::UpdateBuffers(ID3D11DeviceContext* devcon, Matrices sceneInfo, DirectX::XMVECTOR campos)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	DataType* dataPtr2;

	if (FAILED(devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return S_FALSE;

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = DirectX::XMMatrixTranspose(sceneInfo.worldMatrix);
	dataPtr->view = DirectX::XMMatrixTranspose(sceneInfo.viewMatrix);
	dataPtr->projection = DirectX::XMMatrixTranspose(sceneInfo.projectionMatrix);

	// Unlock the constant buffer.
	devcon->Unmap(m_matrixBuffer, 0);

	if (FAILED(devcon->Map(m_dataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return S_FALSE;

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (DataType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr2->campos.x = campos.m128_f32[0];
	dataPtr2->campos.y = campos.m128_f32[1];
	dataPtr2->campos.z = campos.m128_f32[2];
	dataPtr2->campos.w = 1.0f;

	// Unlock the constant buffer.
	devcon->Unmap(m_dataBuffer, 0);
	return S_OK;
}
