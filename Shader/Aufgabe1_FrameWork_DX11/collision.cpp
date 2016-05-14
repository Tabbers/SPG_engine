#include "collision.h"
#include <DirectXCollision.h>
#include <string>
#include <algorithm>
Collision::Collision()
{
}

Collision::~Collision()
{

}

void Collision::Triangles(D3Dmodel* model, XMMATRIX world)
{
	AddTriangles(model->ModelAsTriangles(world));
}

void Collision::AddTriangles(std::vector<Triangle> triangles)
{
	for each (Triangle var in triangles)
	{
		m_triangles.push_back(var);
	}
}
bool Collision::RayTriangleIntersection(const XMVECTOR& pos, const XMVECTOR& direction, const Triangle triangle, XMVECTOR& hit)
{
	XMVECTOR edge1 = triangle.v2 - triangle.v1;
	XMVECTOR edge2 = triangle.v3 - triangle.v1;

	XMVECTOR p = XMVector3Cross(edge2,direction);
	XMVECTOR detV = XMVector3Dot(p,edge1);
	float det = detV.m128_f32[0];
	// the rayu lies within the plane of the triangle if the determinant is near zero
	if (det > -0.000001f && det < 0.000001f)
	{
		return false;
	}

	float invDet = 1.0f / det;
	XMVECTOR t = pos - triangle.v1;
	XMVECTOR uv = XMVector3Dot(p, t);;
	float u = uv.m128_f32[0] * invDet;
	// check whether the intersection is outside the triangle
	if (u < 0.0f || u > 1.0f)
	{
		return false;
	}

	XMVECTOR q = XMVector3Cross(edge1, t);
	XMVECTOR vv = XMVector3Dot(q,direction);
	float v = vv.m128_f32[0] * invDet;
	if (v < 0.0f || v > 1.0f)
	{
		return false;
	}

	XMVECTOR rv = XMVector3Dot(q,edge2);
	float r = rv.m128_f32[0]*invDet;
	if (r > 0.000001f)
	{
		hit = pos + (direction * r);
		return true;
	}

	return false;
}
bool Collision::RayTriangleIntersection(Ray ray, const Triangle tri, float &length)
{
	float intersectDistance = 0;
	bool intersection = DirectX::TriangleTests::Intersects(ray.m_Origin, ray.m_Direction, tri.v1, tri.v2, tri.v3, intersectDistance);
	length = intersectDistance;
	return intersection;
}
bool Collision::CheckCollision(Ray ray, float &length, int  &triangleindex,XMVECTOR &PointHit)
{
	bool res = false;
	float resLength = std::numeric_limits<float>::infinity();
	int hitIndex = std::numeric_limits<int>::infinity();
	int i = 0;
	for each (Triangle tri in m_triangles)
	{
		float intersectDistance = 0;
		bool intersection = false;
		intersection = DirectX::TriangleTests::Intersects(ray.m_Origin, ray.m_Direction, tri.v1, tri.v2, tri.v3, intersectDistance);
		if (intersection && intersectDistance < resLength)
		{
			res = intersection;
			resLength = intersectDistance;
			hitIndex = i;
		}
		i++;
	}
	PointHit = XMVectorAdd(ray.m_Origin, ray.m_Direction*resLength);
	length = resLength;
	triangleindex = hitIndex;
	return res;
}
Ray Collision::MouseToWWorld(int mouseX, int mouseY, int screenWidth, int screenHeight, Matrices sceneinfo, XMVECTOR cameraPosition)
{
	XMVECTOR pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR pickRayInViewSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR rayDirection = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	float PRVecX, PRVecY, PRVecZ;
	XMFLOAT4X4 project;
	XMStoreFloat4x4(&project, sceneinfo.projectionMatrix);

	//Transform 2D pick position on screen space to 3D ray in View space
	PRVecX = (((2.0f * mouseX) / screenWidth) - 1) / project._11;
	PRVecY = -(((2.0f * mouseY) / screenHeight) - 1) / project._22;
	PRVecZ = 1.0f;    //View space's Z direction ranges from 0 to 1, so we set 1 since the ray goes "into" the screen

	pickRayInViewSpaceDir = XMVectorSet(PRVecX, PRVecY, PRVecZ, 0.0f);

	// Transform 3D Ray from View space to 3D ray in World space
	XMMATRIX inverseMatrix;
	XMVECTOR matInvDeter;

	inverseMatrix = XMMatrixInverse(&matInvDeter, sceneinfo.viewMatrix);    //Inverse of View Space matrix is World space matrix

	rayOrigin = XMVector3TransformCoord(pickRayInViewSpacePos, inverseMatrix);  // camera position
	rayDirection = XMVector3TransformNormal(pickRayInViewSpaceDir, inverseMatrix);

	rayDirection = XMVector3Normalize(rayDirection);
	
	return Ray(rayOrigin, rayDirection);
}
Ray Collision::MouseToWorld(int mouseX, int mouseY, int screenWidth, int screenHeight, Matrices sceneinfo, XMVECTOR cameraPosition)
{
	float vX, vY, vZ;
	XMVECTOR worldRayPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR worldRayDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	
	XMFLOAT4X4 project;
	XMStoreFloat4x4(&project,sceneinfo.projectionMatrix);

	//Transform 2D pick position on screen space to 3D ray in View space
	vX = (2.0f * mouseX / screenWidth - 1.0f )/ project._11;
	vY = -(2.0f * mouseY / screenHeight - 1.0f) / project._22;
	vZ = 1.0f;	//View space's Z direction ranges from 0 to 1, so we set 1 since the ray goes "into" the screen

	OutputDebugStringA(std::to_string(vX).c_str());
	OutputDebugStringA("::");
	OutputDebugStringA(std::to_string(vY).c_str());
	OutputDebugStringA("\n");

	// Adjust the points using the projection matrix to account for the aspect ratio of the viewport.
	XMFLOAT4X4 viewInverse;
	XMMATRIX viewinv;
	viewinv = XMMatrixInverse(NULL, sceneinfo.viewMatrix);
	XMStoreFloat4x4(&viewInverse,viewinv);

	// Calculate the direction of the picking ray in view space.
	float x = (vX * viewInverse._11) + (vX * viewInverse._21) + viewInverse._31;
	float y = (vY * viewInverse._12) + (vY * viewInverse._22) + viewInverse._32;
	float z = (vZ * viewInverse._13) + (vZ * viewInverse._23) + viewInverse._33;

	worldRayDir = XMVectorSet(x,y,z,0);
	worldRayPos = cameraPosition;
	worldRayDir = XMVector4Normalize(worldRayDir);

	return Ray(worldRayPos, worldRayDir);
}
