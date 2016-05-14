#include "line.h"



Line::Line()
{
}


Line::~Line()
{
}

bool Line::Init(ID3D11Device * device, XMVECTOR Point1, XMVECTOR Point2, XMFLOAT3 color)
{
	bool result;
	result = InitBuffers(device,Point1,Point2,color);
	if (!result) return false;

	return true;
}

void Line::Render(ID3D11DeviceContext *devCon)
{
	RenderBuffers(devCon);
	return;
}

bool Line::InitBuffers(ID3D11Device* device,XMVECTOR s, XMVECTOR e, XMFLOAT3 color)
{
	HRESULT result;
	VertexType* vertices;
	unsigned long* indices = new unsigned long[m_indexCount];
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	XMFLOAT3 start = { s.m128_f32[0],s.m128_f32[1] ,s.m128_f32[2] };
	XMFLOAT3 end = { e.m128_f32[0] ,e.m128_f32[1] ,e.m128_f32[2] };

	// Array for vertices
	vertices = new VertexType[m_vertexCount];
	if (!vertices) return false;
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].color = color;
		indices[i] = i;
	}
	vertices[0].position = start;
	vertices[1].position = end;
	// Setup dec vor vert buffer with size of vertextype*count
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

	// Release arrays
	delete[] vertices;
	vertices = 0;

	return true;
}
void Line::UpdateLine(ID3D11DeviceContext* devCon,XMVECTOR s,XMVECTOR e)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* vertices;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	XMFLOAT3 color = { 0,255,0 };

	XMFLOAT3 start = {s.m128_f32[0],s.m128_f32[1] ,s.m128_f32[2] };
	XMFLOAT3 end = { e.m128_f32[0] ,e.m128_f32[1] ,e.m128_f32[2] };

	vertices = new VertexType[m_vertexCount];
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].color = color;
	}
	vertices[1].position = end;

	//	Disable GPU access to the vertex buffer data.
	devCon->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//	Update the vertex buffer here.
	memcpy(mappedResource.pData, vertices, sizeof(vertices));
	//	Reenable GPU access to the vertex buffer data.
	devCon->Unmap(m_vertexBuffer, 0);
}

void Line::ShutdownBuffers()
{
	// Release the index and vert buffer
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	return;
}

void Line::RenderBuffers(ID3D11DeviceContext *devCon)
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
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}
