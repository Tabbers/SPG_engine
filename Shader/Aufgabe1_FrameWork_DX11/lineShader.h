#pragma once
#ifndef _LINESHADER_H_
#define _LINESHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "globaldefinitions.h"

class LineShader
{
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

public:
	LineShader();
	~LineShader();
	bool SetShaderParameters(ID3D11DeviceContext * devcon, ID3D11Device * device, Matrices sceneInfo);
	bool Init(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, ID3D11Device*, int, Matrices sceneInfo);

private:
	bool InitShader(ID3D11Device*, HWND, CHAR*, CHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, CHAR*);
	void RenderShader(ID3D11DeviceContext*, int);

	ID3D11Buffer* matrixBuffer;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
};
#endif

