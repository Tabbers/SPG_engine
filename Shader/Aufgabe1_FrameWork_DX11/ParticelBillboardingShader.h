#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "BufferTypes.h"
#include "globaldefinitions.h"

class ParticelBillboardingShader
{
	struct DataType
	{
		DirectX::XMFLOAT4 campos;
	};

	public:
		ParticelBillboardingShader();
		~ParticelBillboardingShader();

		HRESULT Init(ID3D11DeviceContext*, ID3D11Device*, const char* , const char* , const char* );
		HRESULT Render(ID3D11DeviceContext*, ID3D11Device*, Matrices, ID3D11Buffer*, ID3D11ShaderResourceView*, DirectX::XMVECTOR campos);
		HRESULT CompileShaderFromFile(WCHAR * FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob ** Outblob);
		HRESULT UpdateBuffers(ID3D11DeviceContext * devcon, Matrices sceneInfo, DirectX::XMVECTOR campos);
	private:
		ID3D11VertexShader*	  m_VertexShader   = nullptr;
		ID3D11GeometryShader* m_GeometryShader = nullptr;
		ID3D11PixelShader*	  m_PixelShader    = nullptr;
		ID3D11InputLayout*    m_VertexLayout   = nullptr;

		ID3D11SamplerState*	  m_SamplerDiffuse = nullptr;
		ID3D11Buffer*		  m_matrixBuffer   = nullptr;
		ID3D11Buffer*		  m_dataBuffer     = nullptr;
};