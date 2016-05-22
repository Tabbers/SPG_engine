#pragma once
#ifndef _COLORSHADER_H_
#define _COLORSHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "globaldefinitions.h"

class ColorShader
{
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;

		DirectX::XMMATRIX lightView;
		DirectX::XMMATRIX lightProjection;
	};
	struct LightBufferType
	{
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT4 specularColor;
		DirectX::XMFLOAT3 lightDirection;
		float specularIntensity;
		DirectX::XMFLOAT4 switches;
		DirectX::XMFLOAT2 stepsXRefiningStepsy;
		DirectX::XMFLOAT2 padding5;
	};

	struct LightBufferType2
	{
		DirectX::XMFLOAT3 lightPosition;
		float padding;
	};
	
	struct CameraBufferType
	{
		DirectX::XMFLOAT3 cameraPosition;
		float padding1;
		DirectX::XMFLOAT3 cameraDirection;
		float padding2;
	};

public:
	ColorShader();
	~ColorShader();

	bool Init(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, ID3D11Device*, int, Matrices, LightData,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*);

	inline void SetFilter(int filter) { m_filter = filter; };
	inline int  GetFilter() { return m_filter; };
	inline D3D11_FILTER GetFilterEnum() { return m_sampleFilter; };

	inline void SetMip(int mipmap) { m_mipmap = mipmap; };
	inline int  GetMip() { return m_mipmap; };

	inline void SetNumberOfIterations(int numberOfIterations) { m_numberOfIterations = numberOfIterations; };
	inline int  GetNumberOfIterations() { return m_numberOfIterations; };

	inline void SetNumberOfIterationsRefined(int refinementIterations) { m_refinementIterations = refinementIterations; };
	inline int  GetNumberOfIterationsRefined() { return m_refinementIterations; };

private:
	bool InitShader(ID3D11Device*, HWND, CHAR*, CHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, CHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, ID3D11Device*, Matrices, LightData,
		ID3D11ShaderResourceView* , ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;

	int m_mipmap = 0;
	int m_filter = 0;
	int m_filtervalues[9] = {0x00,0x1,0x4,0x5,0x10,0x11,0x14,0x15,0x55};
	int m_numberOfIterations   = 10;
	int m_refinementIterations = 5;

	D3D11_FILTER m_sampleFilter;
	ID3D11SamplerState* m_sampleState;
	ID3D11SamplerState* m_sampleStateText;
	ID3D11SamplerState* m_sampleStateNormal;

	// Contstn buffers
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* lightBuffer2;
	ID3D11Buffer* cameraBuffer;
};

#endif