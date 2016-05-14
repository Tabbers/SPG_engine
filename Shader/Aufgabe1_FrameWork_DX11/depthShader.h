#pragma once
#ifndef _DEPTHSHADER_H_
#define _DEPTHSHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

class DepthShader
{
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

public:
	DepthShader();
	~DepthShader();
	bool Init(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX);

private:
	bool InitShader(ID3D11Device*, HWND, CHAR*, CHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, CHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX);
	void RenderShader(ID3D11DeviceContext*, int);

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
};
#endif
