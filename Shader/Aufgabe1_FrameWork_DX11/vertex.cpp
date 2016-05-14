#include "vertex.h"



Vertex::Vertex()
{
}

Vertex::Vertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT2 text, DirectX::XMFLOAT3 normal)
{
	m_normal   = normal;
	m_textcord = text;
	m_position = position;
}


Vertex::~Vertex()
{
}
