#pragma once

#include <D3D11.h>
#include <DirectXMath.h>
#include <noise\noise.h>
extern int g_TriTable[][16];

/*vertex decals description*/
struct OnceBuffer 
{
	DirectX::XMFLOAT4 decal[8];
	DirectX::XMFLOAT4 dataStep;
};


/*
Vertex description
*/
struct SimpleVertex
{
	DirectX::XMFLOAT4 Pos;
	DirectX::XMFLOAT4 Color;
};



class VolumetricData
{
public:
	VolumetricData(int Width, int Height, int Depth, DirectX::XMFLOAT3 CubeSize, DirectX::XMFLOAT3 CubeStep);
	~VolumetricData();

	HRESULT CreateTestData(const char, ID3D11Device* device);
	ID3D11ShaderResourceView* GetShaderResource();
	void GetDecals(OnceBuffer&);
	int GetVertices(SimpleVertex** outVertices);
	int GetVertexCount();
	ID3D11ShaderResourceView* GetTriTableShaderResource();
	HRESULT CreateTriTableResource(ID3D11Device* device);
	
private:
	//Copy construction disabled by making the copy constructor private.
	VolumetricData(const VolumetricData&);

	HRESULT createTexture(ID3D11Device* device);
	HRESULT createShaderResourceView(ID3D11Device* device);
	void createTextureDesc();
	void createSubresourceData();

	//Functions for creating noise data
	void createDataArray();
	void createSphere();
	void createRandomNoise(double);
	void createBumpySphere(double);

	float getDistance(DirectX::XMFLOAT3, DirectX::XMFLOAT3);
	int getIdx(int,int,int);


	noise::module::Perlin m_perlinNoise;
	int m_vertexCount;
	DirectX::XMFLOAT3 m_cubeSize;
	DirectX::XMFLOAT3 m_cubeStep;
	float* m_data;
	ID3D11Texture3D* m_texture;
	ID3D11ShaderResourceView* m_shaderResourceView = NULL;
	ID3D11ShaderResourceView* m_triTableSRV = NULL;
	UINT m_width, m_height, m_depth;
	D3D11_TEXTURE3D_DESC m_texDesc;
	D3D11_SUBRESOURCE_DATA  m_subData;
};



