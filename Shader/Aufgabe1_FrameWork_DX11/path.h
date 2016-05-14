#ifndef _PATH_H_
#define _PATH_H_

#include <vector>
#include <DirectXMath.h>
#include "pathpoint.h" 

class Path
{
public:
	//Path();
	//~Path();

	void AddPoint(DirectX::XMVECTOR, DirectX::XMVECTOR);
	DirectX::XMVECTOR GetPositionOfPoint(int);
	DirectX::XMVECTOR GetRotationOfPoint(int);
	float GetPathSize();
public:
	float length  = 0;
private:
	std::vector<PathPoint> path;
	DirectX::XMVECTOR vector;
};
#endif

