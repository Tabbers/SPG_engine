#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <fstream>
#include "generatedModel.h"
#include "texture.h"
#include "VolumetricData.h"
#include "BufferTypes.h"
#include "globaldefinitions.h"

struct MatricesGen
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;

	DirectX::XMFLOAT4 LightPostion;

	inline MatricesGen(DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection,	DirectX::XMVECTOR LightPostion)
	{
		this->world = world;
		this->view = view;
		this->projection =projection;
		this->LightPostion.x = LightPostion.m128_f32[0];
		this->LightPostion.y = LightPostion.m128_f32[1];
		this->LightPostion.z = LightPostion.m128_f32[2];
		this->LightPostion.w = LightPostion.m128_f32[3];
	}
	~MatricesGen()
	{
	}

};

class GenerationShader
{
public:
	GenerationShader();
	~GenerationShader();

	HRESULT Init(ID3D11Device *device, ID3D11DeviceContext* devcon);
	void Render(ID3D11DeviceContext* devcon, ID3D11Device* device, MatricesGen sceneInfo,
		GeneratedModel* model);

	inline ID3D11Buffer* GetConstantBuffer() { return m_matrixBuffer; };
private:

	HRESULT InitShader(ID3D11Device *device, ID3D11DeviceContext* devcon);
	HRESULT compileShaderFromFile(WCHAR* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** Outblob);

	ID3D11VertexShader*   m_VertexShader   = nullptr;
	ID3D11PixelShader*    m_PixelShader    = nullptr;
	ID3D11GeometryShader* m_GeometryShader = nullptr;

	ID3D11InputLayout*    m_VertexLayout = nullptr;
	ID3D11SamplerState*   m_SamplerPoint = nullptr;
	ID3D11SamplerState*	  m_SamplerText  = nullptr;
	ID3D11Buffer*		  m_matrixBuffer = nullptr;
};

