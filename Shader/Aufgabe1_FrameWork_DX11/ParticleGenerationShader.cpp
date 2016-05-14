#include "ParticleGenerationShader.h"
#include <fstream>

ParticleGenerationShader::ParticleGenerationShader()
{
}

ParticleGenerationShader::~ParticleGenerationShader()
{
}

HRESULT ParticleGenerationShader::Init(unsigned int numberOfParticles, ID3D11Device *device, Particle* src, ID3D11DeviceContext* devcon,
	const char* vsFilename, const char* gsFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage	 	 = nullptr;
	ID3D10Blob* vertexShaderBuffer	 = nullptr; 
	ID3D10Blob* geometryShaderBuffer = nullptr;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[6];
	D3D11_SAMPLER_DESC samplerDesc;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC dataBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	unsigned int numElements = 0;
	wchar_t temp[100];
	
	m_maximumNumberOfParticles = numberOfParticles;

	//Create Buffers

	D3D11_BUFFER_DESC desc;

		// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(Particle) * m_maximumNumberOfParticles;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	if (FAILED(result = device->CreateBuffer(&desc, nullptr, &m_VertexbufferFront)))
		return S_FALSE;
	if (FAILED(result = device->CreateBuffer(&desc, nullptr, &m_VertexbufferBack)))
		return S_FALSE;



	// Setup dec vor vert buffer with size of vertextype*count
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(Particle);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	// pointer to vertiues
	vertexData.pSysMem = src;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(result = device->CreateBuffer(&desc, &vertexData, &m_SingleVertex)))
		return S_FALSE;

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
	dataBufferDesc.ByteWidth = sizeof(DataBufferType);
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

	if(FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),nullptr,&m_VertexShader)))
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
	
	if (FAILED(result = device->CreateInputLayout(polygonLayout, 6u, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_VertexLayout)))
		return S_FALSE;


	swprintf(temp, 100, L"%hs", gsFilename);
	if (FAILED(CompileShaderFromFile(temp, "main", "gs_5_0", &geometryShaderBuffer)))
	{
		return S_FALSE;
	}
	D3D11_SO_DECLARATION_ENTRY pDecl[] =
	{
		{ 0, "POSITION", 0,0,4,0 },
		{ 0, "VELOCITY", 0,0,4,0 },
		{ 0, "SIZE", 0,0,2,0 },
		{ 0, "AGE", 0,0,1,0 },
		{ 0, "INTERVAL", 0,0,1,0 },
		{ 0, "TYPE", 0,0,1,0 },
	};
	
	if (FAILED(result = device->CreateGeometryShaderWithStreamOutput
			(
				geometryShaderBuffer->GetBufferPointer(),
				geometryShaderBuffer->GetBufferSize(),
				pDecl,
				6u,
				nullptr,
				0,
				D3D11_SO_NO_RASTERIZED_STREAM,
				nullptr,
				&m_GeometryShader
			)
	   ))
	return S_FALSE;



	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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

	if (FAILED(device->CreateSamplerState(&samplerDesc, &m_SamplerPoint)))
		return S_FALSE;

	vertexShaderBuffer->Release();
	geometryShaderBuffer->Release();

	return S_OK;
}

HRESULT ParticleGenerationShader::RenderSingle(ID3D11DeviceContext *devcon, ID3D11Device *device, Matrices sceneInfo, ID3D11ShaderResourceView* randomTex, DataBufferType data)
{
	UpdateBuffers(devcon, sceneInfo,data);


	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(Particle);
	unsigned int offset = 0;
	devcon->SOSetTargets(1, &m_VertexbufferBack, &offset);
	devcon->IASetInputLayout(m_VertexLayout);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);


	UINT offset2 = 0u;


	devcon->VSSetShader(m_VertexShader, nullptr, 0);

	devcon->IASetVertexBuffers(0, 1, &m_SingleVertex, &stride, &offset);

	devcon->GSSetShader(m_GeometryShader, nullptr, 0);
	//SetShaderParameters
	devcon->GSSetSamplers(0, 1, &m_SamplerPoint);
	devcon->GSSetConstantBuffers(0, 1, &m_matrixBuffer);
	devcon->GSSetConstantBuffers(1, 1, &m_dataBuffer);
	
	devcon->GSSetShaderResources(0, 1, &randomTex);
	devcon->PSSetShader(nullptr, nullptr, 0);


	devcon->Draw(1,0);
	
	ID3D11Buffer* pBuffer = nullptr;
	devcon->SOSetTargets(1, &pBuffer, &offset2);
	devcon->GSSetShader(nullptr, nullptr, 0);
	devcon->VSSetShader(nullptr, nullptr, 0);
	devcon->PSSetShader(nullptr, nullptr, 0);

	return S_OK;
}

HRESULT ParticleGenerationShader::Render(ID3D11DeviceContext *devcon, ID3D11Device *device, Matrices sceneInfo, ID3D11ShaderResourceView* randomTex, DataBufferType data)
{
	UpdateBuffers(devcon, sceneInfo, data);

	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(Particle);
	unsigned int offset = 0;

	UINT offset2 = 0u;

	devcon->SOSetTargets(1, &m_VertexbufferBack, &offset);
	devcon->IASetInputLayout(m_VertexLayout);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	devcon->VSSetShader(m_VertexShader, nullptr, 0);
	// Set the vertex buffer to active in the input assembler so it can be rendered.

	devcon->IASetVertexBuffers(0, 1, &m_VertexbufferFront, &stride, &offset);

	devcon->GSSetShader(m_GeometryShader, nullptr, 0);
	//SetShaderParameters
	devcon->GSSetSamplers(0,1,&m_SamplerPoint);
	devcon->GSSetConstantBuffers(0, 1, &m_matrixBuffer);
	devcon->GSSetConstantBuffers(1, 1, &m_dataBuffer);
	devcon->GSSetShaderResources(0, 1, &randomTex);
	devcon->PSSetShader(nullptr, nullptr, 0);

	devcon->DrawAuto();

	ID3D11Buffer* pBuffer = nullptr;
	devcon->SOSetTargets(1, &pBuffer, &offset2);
	devcon->GSSetShader(nullptr, nullptr, 0);
	devcon->VSSetShader(nullptr, nullptr, 0);
	devcon->PSSetShader(nullptr, nullptr, 0);
	return S_OK;
}

HRESULT ParticleGenerationShader::CompileShaderFromFile(WCHAR * FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob ** Outblob)
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

HRESULT ParticleGenerationShader::UpdateBuffers(ID3D11DeviceContext* devcon, Matrices sceneInfo, DataBufferType data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	DataBufferType* dataPtr2;

	if (FAILED(devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return S_FALSE;

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = sceneInfo.worldMatrix;
	dataPtr->view = sceneInfo.viewMatrix;
	dataPtr->projection = sceneInfo.viewMatrix;

	// Unlock the constant buffer.
	devcon->Unmap(m_matrixBuffer, 0);


	if (FAILED(devcon->Map(m_dataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return S_FALSE;

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (DataBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr2->Data = data.Data;
	dataPtr2->Random = data.Random;

	// Unlock the constant buffer.
	devcon->Unmap(m_dataBuffer, 0);

	return S_OK;
}
