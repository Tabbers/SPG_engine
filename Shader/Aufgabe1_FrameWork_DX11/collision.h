#pragma once
#include "globaldefinitions.h"
#include "d3dmodel.h"
#include "d3dcamera.h"

class Collision
{
public:
	Collision();
	~Collision();
	void Triangles(D3Dmodel* model, XMMATRIX world);
	bool CheckCollision(Ray, float &, int &, XMVECTOR &);
	bool RayTriangleIntersection(const XMVECTOR & pos, const XMVECTOR & direction, const Triangle triangle, XMVECTOR & hit);
	bool RayTriangleIntersection(Ray ray, const Triangle tri, float & length);
	Ray MouseToWWorld(int mouseX, int mouseY, int screenWidth, int screenHeight, Matrices sceneinfo, XMVECTOR cameraPosition);
	Ray MouseToWorld(int mouseX, int mouseY, int screenWidth, int screenHeight, Matrices sceneinfo, XMVECTOR);
	inline Triangle GetTriangle(int i) { return m_triangles[i]; };
	inline std::vector<Triangle> GetTriangles() { return m_triangles; };
	inline std::vector<Triangle*> GetTrianglesAsPointers() {
		std::vector<Triangle*>temp;
		for (int i = 0; i < m_triangles.size();i++)
		{
			temp.push_back(&m_triangles[i]);
		}
		return temp;
	};
	inline void ResetTriangle(int i)
	{
		m_triangles[i].intersection = false;
	}
private:
	int NumberOfDimensions = 3;
	void AddTriangles(std::vector<Triangle>);
	std::vector<Triangle> m_triangles;
};


