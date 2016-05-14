#include "d3dmath.h"

D3DMath::D3DMath()
{
}


D3DMath::~D3DMath()
{
}

DirectX::XMVECTOR D3DMath::KochanekBartels(float t, float a, float b, float c, DirectX::XMVECTOR p1, DirectX::XMVECTOR p2, DirectX::XMVECTOR p3, DirectX::XMVECTOR p4)
{
	using namespace DirectX;
	XMVECTOR point;
	XMVECTOR si;
	XMVECTOR di;

	si = (((1 - a)*(1 + b)*(1 - c)) / 2)*(p3 - p2) + (((1 - a)*(1 - b)*(1 + c)) / 2)*(p4 - p3);
	di = (((1 - a)*(1 + b)*(1 + c)) / 2)*(p2 - p1) + (((1 - a)*(1 - b)*(1 - c)) / 2)*(p3 - p2);

	point = (2 * t*t*t - 3 * t*t + 1)*p2 + (t*t*t - 2 * t*t + t)*di + (t*t*t - t*t)*si + (-2 * t*t*t + 3 * t*t)*p3;

	return point;
}
DirectX::XMVECTOR D3DMath::Squad(float t, DirectX::XMVECTOR q1, DirectX::XMVECTOR q2, DirectX::XMVECTOR q3, DirectX::XMVECTOR q4)
{
	using namespace DirectX;
	XMVECTOR rotation;
	XMVECTOR setupA;
	XMVECTOR setupB;
	XMVECTOR setupC;

	q1 = XMQuaternionNormalize(q1);
	q2 = XMQuaternionNormalize(q2);
	q3 = XMQuaternionNormalize(q3);
	q4 = XMQuaternionNormalize(q4);

	XMQuaternionSquadSetup(&setupA, &setupB, &setupC, q1, q2, q3, q4);
	rotation = XMQuaternionSquad(q2, setupA, setupB, setupC, t);

	return rotation;
}
