#pragma once 

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "BufferTypes.h"
#include "globaldefinitions.h"
struct DataBufferType
{
	//Gametime|garvity|initalVelocity|maximumLifetime
	DirectX::XMFLOAT4 Data;
	DirectX::XMFLOAT4 Random;
};

class ParticleGenerationShader
{
	public:
		ParticleGenerationShader();
		~ParticleGenerationShader();
		inline ID3D11Buffer* GetVertexBuffer() { return m_VertexbufferBack; }

		HRESULT Init(unsigned int, ID3D11Device*, Particle*, ID3D11DeviceContext*,const char*, const char*);
		HRESULT RenderSingle(ID3D11DeviceContext*, ID3D11Device*,  Matrices, ID3D11ShaderResourceView*, DataBufferType);
		HRESULT Render(ID3D11DeviceContext*, ID3D11Device*, Matrices, ID3D11ShaderResourceView*, DataBufferType);
		inline void SwapBuffer() { std::swap(m_VertexbufferFront, m_VertexbufferBack); };
	private:
		HRESULT CompileShaderFromFile(WCHAR *, LPCSTR, LPCSTR, ID3DBlob **);
		HRESULT UpdateBuffers(ID3D11DeviceContext*, Matrices, DataBufferType);
	private:
		ID3D11VertexShader*	  m_VertexShader   = nullptr;
		ID3D11GeometryShader* m_GeometryShader = nullptr;
		ID3D11InputLayout*    m_VertexLayout   = nullptr;

		ID3D11Buffer* m_VertexbufferFront	   = nullptr;
		ID3D11Buffer* m_VertexbufferBack	   = nullptr;
		ID3D11Buffer* m_SingleVertex		   = nullptr;

		ID3D11SamplerState*	  m_SamplerPoint   = nullptr;
		ID3D11Buffer*		  m_matrixBuffer   = nullptr;
		ID3D11Buffer*		  m_dataBuffer     = nullptr;
		
		unsigned int m_maximumNumberOfParticles = 0;
};