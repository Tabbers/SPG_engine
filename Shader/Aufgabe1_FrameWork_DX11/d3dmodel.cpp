#include "d3dmodel.h"
#include "model.h"
#include "vertex.h"
#include "texture.h"
#include "modellibrary.h"
#include <string>


D3Dmodel::D3Dmodel(ModelLibrary* lib):m_vertexBuffer(nullptr),m_indexBuffer(nullptr),m_modelLib(nullptr),m_Text(nullptr),m_NormalMap(nullptr)
{
	this->m_modelLib = lib;
}

D3Dmodel::D3Dmodel(const D3Dmodel &other)
{
}


D3Dmodel::~D3Dmodel()
{
	ShutdownBuffers();
	m_modelLib = nullptr;
	if (m_Text != nullptr)
	{
		delete m_Text;
	}
	if (m_NormalMap != nullptr)
	{
		delete m_NormalMap;
	}
}

bool D3Dmodel::Init(char* locationModel,wchar_t* locationTexture, wchar_t* locationNormal, wchar_t* locationDisp, ID3D11Device * device, ID3D11DeviceContext* devcon, XMVECTOR position, XMVECTOR rotation)
{
	bool result = true;
	int modelid;
	modelid = m_modelLib->CheckLibrary(locationModel);
	if (modelid == -1)
	{
		if (locationModel == "") m_modelid = 0;
		else m_modelLib->LoadModel(locationModel,m_modelid);
	}
	else
	{
		m_modelid = modelid;
	}

	m_Text = new Texture();
	if (locationTexture == L"") result = m_Text->Init(device, devcon, L"Texture/texture.dds");
	else result = m_Text->Init(device,devcon,locationTexture);

	m_NormalMap = new Texture();
	if (locationNormal == L"") result = m_NormalMap->Init(device, devcon, L"Texture/normal.dds");
	else result = m_NormalMap->Init(device, devcon, locationNormal);

	m_DisplacementMap = new Texture();
	if (locationDisp == L"") result = m_DisplacementMap->Init(device, devcon, L"Texture/displacement.dds");
	else result = m_DisplacementMap->Init(device, devcon, locationDisp);

	CalculateModelVectors();

	result = InitBuffers(device, position, rotation);
	if (!result) return false;
	return result;
}

void D3Dmodel::Render(ID3D11DeviceContext * devCon)
{
	RenderBuffers(devCon);
	return;
}

std::vector<Triangle> D3Dmodel::ModelAsTriangles( XMMATRIX world)
{
	Model* model = m_modelLib->GetModel(m_modelid);
	std::vector<Triangle> triangles;

	for (int i = 0; i < m_vertexCount; i+=3)
	{
		XMVECTOR Trianglevertex1;
		XMVECTOR Trianglevertex2;
		XMVECTOR Trianglevertex3;
		
		Trianglevertex1 = XMVectorSet(model->vertices[i].m_position.x,   model->vertices[i].m_position.y,   model->vertices[i].m_position.z, 0);
		Trianglevertex2 = XMVectorSet(model->vertices[i+1].m_position.x, model->vertices[i+1].m_position.y, model->vertices[i+1].m_position.z, 0);
		Trianglevertex3 = XMVectorSet(model->vertices[i+2].m_position.x, model->vertices[i+2].m_position.y, model->vertices[i+2].m_position.z, 0);

		Trianglevertex1 = XMVector3Transform(Trianglevertex1, world);
		Trianglevertex2 = XMVector3Transform(Trianglevertex2, world);
		Trianglevertex3 = XMVector3Transform(Trianglevertex3, world);

		Triangle tri(Trianglevertex1,Trianglevertex2,Trianglevertex3);
		tri.CalculateCenter();
		triangles.push_back(tri);
	}

	return triangles;
}

int D3Dmodel::GetIndexCount()
{
	return m_indexCount;
}

XMMATRIX D3Dmodel::adjustWorldmatrix(XMMATRIX worldMatrix)
{
	m_worldmatrix = worldMatrix;

	XMMATRIX scale = XMMatrixScalingFromVector(m_scale);
	XMMATRIX rotate = XMMatrixRotationQuaternion(m_rotation);
	XMMATRIX position = XMMatrixTranslationFromVector(m_position);

	m_worldmatrix = XMMatrixMultiply(m_worldmatrix, scale);
	m_worldmatrix = XMMatrixMultiply(m_worldmatrix, rotate);
	m_worldmatrix = XMMatrixMultiply(m_worldmatrix, position);
	worldMatrix = XMMatrixMultiply(worldMatrix,m_worldmatrix);
	return worldMatrix;
}

bool D3Dmodel::InitBuffers(ID3D11Device * device, XMVECTOR position, XMVECTOR rotation)
{
	Model* model = m_modelLib->GetModel(m_modelid);
	this->m_position = position;
	this->m_rotation = rotation;
	HRESULT result;
	VertexType* vertices;
	XMFLOAT3 positions;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	m_vertexCount = model->m_vertexcount;
	m_indexCount  = model->m_indexcount;

	// Array for vertices
	vertices = new VertexType[m_vertexCount];
	if (!vertices) return false;
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = model->vertices[i].m_position;
		vertices[i].texture  = model->vertices[i].m_textcord;
		vertices[i].normal   = model->vertices[i].m_normal;
		vertices[i].tangent  = model->vertices[i].m_tangent;
		vertices[i].binormal = model->vertices[i].m_binormal;
	}

	// Setup dec vor vert buffer with size of vertextype*count
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
	indexData.pSysMem = model->indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) return false;

	// Release arrays
	if (vertices != nullptr)
	{
		delete[] vertices;
	}

	return true;
}

void D3Dmodel::ShutdownBuffers()
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

void D3Dmodel::RenderBuffers(ID3D11DeviceContext * devCon)
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
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
void D3Dmodel::CalculateModelVectors()
{
	Model* model = m_modelLib->GetModel(m_modelid);
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	XMFLOAT3 tangent, binormal, normal;


	// Calculate the number of faces in the model.
	faceCount = model->m_vertexcount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i<faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.position = model->vertices[index].m_position;
		vertex1.texture  = model->vertices[index].m_textcord;
		vertex1.normal   = model->vertices[index].m_normal;
		index++;

		vertex2.position = model->vertices[index].m_position;
		vertex2.texture  = model->vertices[index].m_textcord;
		vertex2.normal   = model->vertices[index].m_normal;
		index++;

		vertex3.position = model->vertices[index].m_position;
		vertex3.texture  = model->vertices[index].m_textcord;
		vertex3.normal   = model->vertices[index].m_normal;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		model->vertices[index - 1].m_normal = normal;
		model->vertices[index - 1].m_tangent = tangent;
		model->vertices[index - 1].m_binormal = binormal;

		model->vertices[index - 2].m_normal = normal;
		model->vertices[index - 2].m_tangent = tangent;
		model->vertices[index - 2].m_binormal = binormal;

		model->vertices[index - 3].m_normal = normal;
		model->vertices[index - 3].m_tangent = tangent;
		model->vertices[index - 3].m_binormal = binormal;
	}

	return;
}

void D3Dmodel::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,
	XMFLOAT3& tangent, XMFLOAT3& binormal)
{
	XMFLOAT3 vector1, vector2;
	XMFLOAT2 tVector, tVector2;
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1.x = vertex2.position.x - vertex1.position.x;
	vector1.y = vertex2.position.y - vertex1.position.y;
	vector1.z = vertex2.position.z - vertex1.position.z;

	vector2.x = vertex3.position.x - vertex1.position.x;
	vector2.y = vertex3.position.y - vertex1.position.y;
	vector2.z = vertex3.position.z - vertex1.position.z;

	// Calculate the tu and tv texture space vectors.
	tVector.x = vertex2.texture.x - vertex1.texture.x;
	tVector.y = vertex2.texture.y - vertex1.texture.y;

	tVector2.x = vertex3.texture.x - vertex1.texture.x;
	tVector2.y = vertex3.texture.y - vertex1.texture.y;

	// Calculate the denominator of the tangent/binormal equation.
	float f = (tVector.x * tVector2.y - tVector2.x * tVector.y);
	if (f > 0)
	{
		den = 1.0f / f;
	}
	else
	{
		den = 1.0f;
	}

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tVector2.y * vector1.x - tVector.y * vector2.x) * den;
	tangent.y = (tVector2.y * vector1.y - tVector.y * vector2.y) * den;
	tangent.z = (tVector2.y * vector1.z - tVector.y * vector2.z) * den;

	binormal.x = (tVector.x * vector2.x - tVector2.x * vector1.x) * den;
	binormal.y = (tVector.x * vector2.y - tVector2.x * vector1.y) * den;
	binormal.z = (tVector.x * vector2.z - tVector2.x * vector1.z) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	if (length > 0)
	{
		// Normalize the normal and then store it
		tangent.x = tangent.x / length;
		tangent.y = tangent.y / length;
		tangent.z = tangent.z / length;
	}
	else
	{
		tangent.x = 0;
		tangent.y = 0;
		tangent.z = 0;
	}

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	if (length > 0)
	{
		// Normalize the normal and then store it
		binormal.x = binormal.x / length;
		binormal.y = binormal.y / length;
		binormal.z = binormal.z / length;
	}
	else
	{
		binormal.x = 0;
		binormal.y = 0;
		binormal.z = 0;
	}
	// Normalize the normal and then store it

	return;
}

void D3Dmodel::CalculateNormal(XMFLOAT3 tangent, XMFLOAT3 binormal, XMFLOAT3& normal)
{
	float length;

	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	if (length > 0)
	{
		// Normalize the normal.
		normal.x = normal.x / length;
		normal.y = normal.y / length;
		normal.z = normal.z / length;

	}
	else
	{
		normal.x = 0;
		normal.y = 0;
		normal.z = 0;
	}

	return;
}