#pragma once
#include <DirectXMath.h> 

struct MatrixBufferType
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

struct Particle
{
	DirectX::XMFLOAT4 InitialPos;
	DirectX::XMFLOAT4 InitialVel;
	DirectX::XMFLOAT2 Size;
	float Age;
	float Interval;
	unsigned int Type;
};