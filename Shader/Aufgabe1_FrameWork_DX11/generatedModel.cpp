#include "generatedModel.h"
#include "VolumetricData.h"

GeneratedModel::GeneratedModel()
{
}

GeneratedModel::~GeneratedModel()
{
}

HRESULT GeneratedModel::Init(char type, ID3D11Device * device, ID3D11DeviceContext * devcon)
{
	int height, depth, width;
	height = depth = width = 64;

	m_dataStep = DirectX::XMFLOAT4(1.0f / (float)width, 1.0f / (float)height, 1.0f / (float)depth, 1);

	DirectX::XMFLOAT3 cubeSize(32.0f, 32.0f, 32.0f);
	//2.0f to decrease density
	DirectX::XMFLOAT3 cubeStep(2.0f / cubeSize.x, 2.0f / cubeSize.y, 2.0f / cubeSize.z);

	if (m_vol != nullptr)
	{
		delete m_vol;
		m_vol= NULL;
	}
	m_vol = new VolumetricData(width, height, depth, cubeSize, cubeStep);
	HRESULT  hr;
	if (FAILED(hr = m_vol->CreateTestData(type,device)))
	{
		if (m_vol != nullptr)
		{
			delete m_vol;
		}
		return hr;
	}

	m_DensityData = m_vol->GetShaderResource();

	if (FAILED(hr = m_vol->CreateTriTableResource(device)))
	{
		if (m_vol != nullptr)
		{
			delete m_vol;
		}
		return hr;
	}

	m_TriTableSRV = m_vol->GetTriTableShaderResource();

	//Create decal buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(OnceBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = device->CreateBuffer(&bd, NULL, &m_SampleBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	OnceBuffer dbuffer;
	m_vol->GetDecals(dbuffer);
	dbuffer.dataStep = m_dataStep;
	devcon->UpdateSubresource(m_SampleBuffer, 0, NULL, &dbuffer, 0, 0);

	InitBuffers(device, devcon);

	m_textureX = new Texture();
	m_textureY = new Texture();
	m_textureZ = new Texture();
	
	m_textureX->Init(device, devcon, L"Texture/texture.dds");
	m_textureY->Init(device, devcon, L"Texture/stones/text_ground.dds");
	m_textureZ->Init(device, devcon, L"Texture/brick/text_brick.dds");

	return S_OK;
}

HRESULT GeneratedModel::InitBuffers(ID3D11Device * device, ID3D11DeviceContext * devcon)
{
	SimpleVertex* vertices = NULL;
	int vertexCount = m_vol->GetVertices(&vertices);

	//Vertex buffer description
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	//This buffer will be optimized to gpu space where cpu doesn't have access
	bd.CPUAccessFlags = 0;

	//Vertex buffer initial data description.
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	HRESULT hr = device->CreateBuffer(&bd, &initData, &m_VertexBuffer);

	if (FAILED(hr))
		return hr;

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	//Create SO stage buffer
	ID3D11Buffer* soBuffer;
	int soBufferSize = 1000000;
	D3D11_BUFFER_DESC soBufferDesc =
	{
		soBufferSize,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_STREAM_OUTPUT,
		0,
		0,
		0
	};
	hr = device->CreateBuffer(&soBufferDesc, NULL, &soBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}
