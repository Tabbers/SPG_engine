#include "Terrain.h"



Terrain::Terrain()
{
}


Terrain::~Terrain()
{
}

XMMATRIX Terrain::adjustWorldmatrix(XMMATRIX worldMatrix)
{
	m_worldmatrix = worldMatrix;

	XMMATRIX scale = XMMatrixScalingFromVector(m_scale);
	XMMATRIX rotate = XMMatrixRotationQuaternion(m_rotation);
	XMMATRIX position = XMMatrixTranslationFromVector(m_position);

	m_worldmatrix = XMMatrixMultiply(m_worldmatrix, scale);
	m_worldmatrix = XMMatrixMultiply(m_worldmatrix, rotate);
	m_worldmatrix = XMMatrixMultiply(m_worldmatrix, position);
	worldMatrix = XMMatrixMultiply(worldMatrix, m_worldmatrix);
	return worldMatrix;
}

bool Terrain::Init(wchar_t * locationTexture, wchar_t * locationDisp, ID3D11Device * device, ID3D11DeviceContext * devcon, XMVECTOR position, XMVECTOR rotation, unsigned int  dimension, unsigned int subDivisions)
{
	m_position = position;

	bool result = true;
	ulong* indices;
	VertexType* vertices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	m_Text = new Texture();
	if (locationTexture == L"") result = m_Text->Init(device, devcon, L"Texture/texture.dds");
	else result = m_Text->Init(device, devcon, locationTexture);

	m_DisplacementMap = new Texture();
	if (locationDisp == L"") result = m_DisplacementMap->Init(device, devcon, L"Texture/displacement.dds");
	else result = m_DisplacementMap->Init(device, devcon, locationDisp);

	m_vertexCount = (subDivisions + 1u) * (subDivisions + 1u);
	vertices = new VertexType[m_vertexCount];

	const float stepLength = dimension / static_cast<float>(subDivisions);
	const float halfDimension = dimension * 0.5f;
	const uint verticesPerColumn = subDivisions + 1u;
	const float uvStepLength = 1.0f / static_cast<float>(subDivisions);

	for (uint vIndex = 0u; vIndex < m_vertexCount; ++vIndex)
	{
		uint x = vIndex % verticesPerColumn;
		uint z = vIndex / verticesPerColumn;

		vertices[vIndex].position = DirectX::XMFLOAT3(x * stepLength - halfDimension, 0.0f, z * (-stepLength) + halfDimension);
		vertices[vIndex].texture  = DirectX::XMFLOAT2(x * uvStepLength, z * uvStepLength);
	}


	const uint quadCount = subDivisions * subDivisions;
	m_indexCount = 4u * quadCount;
	indices = new ulong[m_indexCount];

	for (uint qIndex = 0u; qIndex < quadCount; ++qIndex)
	{
		uint x = qIndex % subDivisions;
		uint y = qIndex / subDivisions;

		ulong topLeftCorner = y * verticesPerColumn + x;
		ulong topRightCorner = y * verticesPerColumn + x + 1u;
		ulong bottomLeftCorner = (y + 1u) * verticesPerColumn + x;
		ulong bottomRightCorner = (y + 1u) * verticesPerColumn + x + 1u;

		indices[qIndex * 4u] = topLeftCorner;
		indices[qIndex * 4u + 1u] = topRightCorner;
		indices[qIndex * 4u + 2u] = bottomRightCorner;
		indices[qIndex * 4u + 3u] = bottomLeftCorner;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// pointer to vertiues
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// create vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) return false;


	// Set up desc for static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// pointer to index Data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) return false;

	if (vertices != nullptr)
	{
		delete[] vertices;
	}
	if (indices != nullptr)
	{
		delete[] indices;
	}

	return result;
}

void Terrain::Render(ID3D11DeviceContext * devCon)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	devCon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	devCon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

}
