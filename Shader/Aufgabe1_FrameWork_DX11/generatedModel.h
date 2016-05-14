#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <fstream>
#include <DirectXMath.h>
#include "texture.h"

class VolumetricData;

class GeneratedModel
{
public:
	GeneratedModel();
	~GeneratedModel();

	HRESULT Init(char type, ID3D11Device* device, ID3D11DeviceContext* devcon);
	//GETTER
	inline ID3D11Buffer* GetVertexBuffer() { return m_VertexBuffer; };
	inline ID3D11Buffer* GetSamplePointBuffer() { return m_SampleBuffer; };

	inline ID3D11ShaderResourceView* GetDensityData() { return m_DensityData; };
	inline ID3D11ShaderResourceView* GetTable() { return m_TriTableSRV; };

	inline VolumetricData* GetVolume() { return m_vol; };
	inline Texture*	getTextureX() { return m_textureX; };
	inline Texture*	getTextureY() { return m_textureY; };
	inline Texture*	getTextureZ() { return m_textureZ; };
private:


	HRESULT InitBuffers(ID3D11Device* device, ID3D11DeviceContext* devcon);

	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_SampleBuffer = nullptr;

	ID3D11ShaderResourceView* m_DensityData = nullptr;
	ID3D11ShaderResourceView* m_TriTableSRV = nullptr;

	VolumetricData* m_vol = nullptr;
	Texture* m_textureX;
	Texture* m_textureY;
	Texture* m_textureZ;
	DirectX::XMFLOAT4 m_dataStep;
};

