#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "globaldefinitions.h"

class TerrainShader
{
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};
public:

	struct TesselationBufferType
	{
		DirectX::XMFLOAT4 edgefactors;
		DirectX::XMFLOAT2 innerfactors;
	};

	TerrainShader();
	~TerrainShader();
	bool Init(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, ID3D11Device*, int, Matrices,	ID3D11ShaderResourceView*, ID3D11ShaderResourceView*);

	inline int GetUniformTesseletionFactor() { return m_uniformTesseletionFactor; }
	inline void SetUniformTesseletionFactor(unsigned int factor) { this->m_uniformTesseletionFactor = factor; }
private:
	bool InitShader(ID3D11Device*, HWND, CHAR*, CHAR*, CHAR*, CHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, CHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, ID3D11Device*, Matrices,	ID3D11ShaderResourceView*, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);
private:
	unsigned int m_uniformTesseletionFactor;

	ID3D11VertexShader* m_vertexShader;
	ID3D11HullShader*	m_hullShader;
	ID3D11DomainShader* m_domainShader;
	ID3D11PixelShader*	m_pixelShader;
	ID3D11InputLayout*  m_layout;

	ID3D11SamplerState* m_samplerText;
	ID3D11SamplerState* m_samplerDisp;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_tesseletionBuffer;
};

