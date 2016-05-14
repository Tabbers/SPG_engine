#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

class Vertex
{
public:
	Vertex();
	Vertex(DirectX::XMFLOAT3, DirectX::XMFLOAT2, DirectX::XMFLOAT3);
	~Vertex();


	//OPERATORS
	inline bool operator==(Vertex &rhs)
	{
		if (this->m_textcord.x == rhs.m_textcord.x &&
			this->m_textcord.y == rhs.m_textcord.y &&
			this->m_normal.x == rhs.m_normal.x &&
			this->m_normal.y == rhs.m_normal.y &&
			this->m_normal.z == rhs.m_normal.z &&
			this->m_position.x == rhs.m_position.x &&
			this->m_position.y == rhs.m_position.y &&
			this->m_position.z == rhs.m_position.z &&
			this->m_binormal.x == rhs.m_binormal.x &&
			this->m_binormal.y == rhs.m_binormal.y &&
			this->m_binormal.z == rhs.m_binormal.z &&
			this->m_tangent.x == rhs.m_tangent.x &&
			this->m_tangent.y == rhs.m_tangent.y &&
			this->m_tangent.z == rhs.m_tangent.z)
			return true;
		else
			return false;
	}
public:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT2 m_textcord;
	DirectX::XMFLOAT3 m_normal;
	DirectX::XMFLOAT3 m_tangent;
	DirectX::XMFLOAT3 m_binormal;
};

