#pragma once
#include "globaldefinitions.h"
#include <DirectXMath.h>
#include <d3d11.h>

using namespace DirectX;
class Line
{
public:
	Line();
	~Line();
	bool HighlightLine = true;
	bool Init(ID3D11Device*, XMVECTOR Point1, XMVECTOR Point2, XMFLOAT3 color);
	void Render(ID3D11DeviceContext*);
	
	void UpdateLine(ID3D11DeviceContext * devCon, XMVECTOR start, XMVECTOR end);
	inline int GetIndexCount()
	{
		return m_indexCount;
	};
private:
	bool InitBuffers(ID3D11Device*, XMVECTOR Point1, XMVECTOR Point2, XMFLOAT3 color);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount = 2;
	int m_indexCount = 2;

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT3 color;
	};
};

