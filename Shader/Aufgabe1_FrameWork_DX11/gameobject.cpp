#include "gameobject.h"



GameObject::GameObject()
{
	m_scale = XMVectorSet(1, 1, 1, 0);
	m_rotation = XMVectorSet(0,0, 0, 0);
}


GameObject::~GameObject()
{
}

void GameObject::SetPosition(float x, float y, float z)
{
	m_position = XMVectorSet(x, y, z, 0.0f);
	return;
}

void GameObject::SetRotation(float x, float y, float z)
{
	m_rotation = XMVectorSet(x, y, z, m_rotation.m128_f32[3]);
	return;
}

XMVECTOR GameObject::GetPosition()
{
	return m_position;
}

XMVECTOR GameObject::GetRotation()
{
	return m_rotation;
}
 