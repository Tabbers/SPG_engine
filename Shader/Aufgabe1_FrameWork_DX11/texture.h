#pragma once

#include <d3d11.h>
#include "DDSTextureLoader.h"

class Texture
{
public:
	Texture();
	~Texture();
	bool Init(ID3D11Device*, ID3D11DeviceContext*, wchar_t*);
	inline ID3D11ShaderResourceView* GetResourceView() { return m_textureView; };
	inline void SetResource(ID3D11Resource* res) { m_texture = res; };
	inline void SetResourceView(ID3D11ShaderResourceView* resView) { m_textureView = resView; };
private:
	ID3D11Resource* m_texture;
	ID3D11ShaderResourceView* m_textureView;
};

