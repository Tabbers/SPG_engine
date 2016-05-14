#ifndef _PATHPOINT_H_
#define _PATHPOINT_H_ 

#include <DirectXMath.h>

class PathPoint
{
public:
	PathPoint();
	~PathPoint();
public:
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR rotation;
};
#endif

