#pragma once
#include "gameobject.h"+
#include "texture.h"

class Terrain : public GameObject
{
private:

	typedef unsigned int uint;
	typedef unsigned long ulong;

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
public:
	Terrain();
	~Terrain();
	XMMATRIX adjustWorldmatrix(XMMATRIX);

	bool Init(wchar_t*, wchar_t*, ID3D11Device*, ID3D11DeviceContext*, XMVECTOR, XMVECTOR, unsigned int, unsigned int);
	void Render(ID3D11DeviceContext*);

	inline int GetIndexCount() { return m_indexCount; }
	inline Texture* GetTexture() { return m_Text; };
	inline Texture* GetDisplacementMap() { return m_DisplacementMap; };
private:
	ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;

	Texture* m_Text;
	Texture* m_DisplacementMap;

	XMMATRIX m_worldmatrix;

};

