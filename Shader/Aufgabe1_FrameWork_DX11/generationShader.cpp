#include "generationShader.h"



GenerationShader::GenerationShader()
{
}


GenerationShader::~GenerationShader()
{
}

HRESULT GenerationShader::Init(ID3D11Device * device, ID3D11DeviceContext * devcon)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferTypeGeneration);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	HRESULT result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) return false;

	if (FAILED(result = InitShader(device, devcon)))
		return result;

	return S_OK;
}

void GenerationShader::Render(ID3D11DeviceContext* devcon, ID3D11Device* device, MatricesGen sceneInfo,GeneratedModel* model)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result;

	MatricesGen* dataPtr;

	ID3D11Buffer* buff = model->GetSamplePointBuffer();
	ID3D11ShaderResourceView* denstiyData = model->GetDensityData();
	ID3D11ShaderResourceView* sampleTable = model->GetTable();
	ID3D11ShaderResourceView* textureX = model->getTextureX()->GetResourceView();
	ID3D11ShaderResourceView* textureY = model->getTextureY()->GetResourceView();
	ID3D11ShaderResourceView* textureZ = model->getTextureZ()->GetResourceView();

	// Transpose the matrices to prepare them for the shader.
	sceneInfo.world = XMMatrixTranspose(sceneInfo.world);
	sceneInfo.view = XMMatrixTranspose(sceneInfo.view);
	sceneInfo.projection = XMMatrixTranspose(sceneInfo.projection);

	result = devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatricesGen*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = sceneInfo.world;
	dataPtr->view = sceneInfo.view;
	dataPtr->projection = sceneInfo.projection;

	dataPtr->LightPostion = sceneInfo.LightPostion;
	// Unlock the constant buffer.
	devcon->Unmap(m_matrixBuffer, 0);

	//Enable vertex shader
	devcon->VSSetShader(m_VertexShader, NULL, 0);

	//Enable geometry shader
	devcon->GSSetShader(m_GeometryShader, NULL, 0);
	//Set constant buffers to use in the geometry shader
	devcon->GSSetConstantBuffers(0, 1, &m_matrixBuffer);
	devcon->GSSetConstantBuffers(1, 1, &buff);
	//Set point sampler to use in the geometry shader
	devcon->GSSetSamplers(0, 1, &m_SamplerPoint);
	//Set textures to use in the geometry shader
	devcon->GSSetShaderResources(0, 1, &sampleTable);
	devcon->GSSetShaderResources(1, 1, &denstiyData);

	//Enable pixel shader
	devcon->PSSetShader(m_PixelShader, NULL, 0);
	devcon->PSSetConstantBuffers(0, 1, &m_matrixBuffer);
	devcon->PSSetConstantBuffers(1, 1, &buff);
	devcon->PSSetShaderResources(0, 1, &denstiyData);
	//Set Textures fr triplanaer texturing
	devcon->PSSetShaderResources(1, 1, &textureX);
	devcon->PSSetShaderResources(2, 1, &textureY);
	devcon->PSSetShaderResources(3, 1, &textureZ);

	devcon->PSSetSamplers(0, 1, &m_SamplerPoint);
	devcon->PSSetSamplers(1, 1, &m_SamplerText);

	//Draw vertices
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	unsigned int vertexCount = model->GetVolume()->GetVertexCount();
	devcon->Draw(vertexCount, 0);

	devcon->GSSetShader(nullptr, nullptr, 0);
	devcon->VSSetShader(nullptr, nullptr, 0);
	devcon->PSSetShader(nullptr, nullptr, 0);
}

HRESULT GenerationShader::InitShader(ID3D11Device * device, ID3D11DeviceContext * devcon)
{

	HRESULT hr = S_OK;

	//Vertex shader
	ID3DBlob* vsBlob = NULL;
	hr = compileShaderFromFile(L"Shaders/MarchingCubesVs.hlsl", "main", "vs_5_0", &vsBlob);

	if (FAILED(hr))
	{
		vsBlob->Release();
		return hr;
	}

	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(), NULL, &m_VertexShader);

	if (FAILED(hr))
	{
		vsBlob->Release();
		return hr;
	}

	//Create input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);
	hr = device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(), &m_VertexLayout);
	vsBlob->Release();
	if (FAILED(hr))
		return hr;
	//set input layout to use
	devcon->IASetInputLayout(m_VertexLayout);


	//compile geometry shader
	ID3DBlob* gsBlob = NULL;
	hr = compileShaderFromFile(L"Shaders/MarchingCubesGs.hlsl", "main", "gs_5_0", &gsBlob);

	if (FAILED(hr))
		return hr;

	hr = device->CreateGeometryShader(gsBlob->GetBufferPointer(),gsBlob->GetBufferSize(), nullptr, &m_GeometryShader);
	if (FAILED(hr))	return S_FALSE;

	//compile and create pixel shader
	ID3DBlob* psBlob = NULL;
	hr = compileShaderFromFile(L"Shaders/MarchingCubesPs.hlsl", "main", "ps_5_0", &psBlob);
	if (FAILED(hr))
		return hr;
	hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &m_PixelShader);
	psBlob->Release();
	if (FAILED(hr))
	{
		return hr;
	}


	//Create a basic point sampler for sampling our density data in the gpu
	//should refactor this elsewhere
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = 0;
	hr = device->CreateSamplerState(&sampDesc, &m_SamplerPoint);
	if (FAILED(hr))
		return hr;

	// Create a texture sampler state description.
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MipLODBias = 0.0f;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.BorderColor[0] = 0;
	sampDesc.BorderColor[1] = 0;
	sampDesc.BorderColor[2] = 0;
	sampDesc.BorderColor[3] = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = device->CreateSamplerState(&sampDesc, &m_SamplerText);
	if (FAILED(hr))
		return hr;


	return S_OK;
}

HRESULT GenerationShader::compileShaderFromFile(WCHAR * FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob ** Outblob)
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
