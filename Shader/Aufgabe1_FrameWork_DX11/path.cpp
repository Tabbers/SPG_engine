#include "path.h"



//Path::Path()
//{
//}


//Path::~Path()
//{
//}

void Path::AddPoint(DirectX::XMVECTOR position, DirectX::XMVECTOR rotation)
{
	using namespace DirectX;
	PathPoint pointTemp;
	pointTemp.position = position;
	pointTemp.rotation = rotation;
	path.push_back(pointTemp);

	XMVECTOR temp;
	for (PathPoint point : path)
	{
		temp += point.position;
	}

	length = XMVector4Length(temp).m128_f32[0];
}

DirectX::XMVECTOR Path::GetPositionOfPoint(int index)
{
	PathPoint pointTemp;
	pointTemp = path[index];

	return pointTemp.position;
}

DirectX::XMVECTOR Path::GetRotationOfPoint(int index)
{
	PathPoint pointTemp;

	pointTemp = path[index];

	return pointTemp.rotation;
}

float Path::GetPathSize()
{
	return path.size();
}
