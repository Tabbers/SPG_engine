#pragma once
#include "globaldefinitions.h"
#include "collision.h"

int irand(int a, int b);

enum SplitDimension {
	X = 0, Y =1 , Z=2
};

class KdNode {
public:
	std::vector<Triangle*>* triangles;
	int dimension;
	float plane;

	KdNode* children[2];

	~KdNode();
};

class KdTree
{
public:
	std::vector<Triangle*>* RenderingTriangles;
	KdNode* root;
	Collision* coll;
public:
	KdTree();
	~KdTree();

	void Init(std::vector<Triangle*>* triangles, ID3D11Device* device, XMFLOAT3 color);
	void Render(ID3D11DeviceContext*);

	KdNode* createTree(std::vector<Triangle*>& triangles, int depth, int numberofTriangles);

	void rayCast(KdNode* node, Ray ray, float &length, float tmax, float tdiff_to_original);

	KdNode* getRoot();

	DirectX::XMVECTOR m_intersectionPoint;
	inline int GetIndexCount()
	{
		return m_indexCount;
	};
private : 
	bool InitBuffers(ID3D11Device*, XMFLOAT3 color);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	//Draw Data for KD Tree
	int m_indexCount = 0;
	std::vector<XMFLOAT3> m_vertices;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT3 color;
	};
};

