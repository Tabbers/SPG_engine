#include "texture.h"



Texture::Texture(): m_texture(nullptr), m_textureView(nullptr)
{
}


Texture::~Texture()
{
	m_texture->Release();
	m_textureView->Release();
}

bool Texture::Init(ID3D11Device *dev, ID3D11DeviceContext *devcon, wchar_t* filename)
{
	using namespace DirectX;
	bool result = true;
	size_t size = 0;
	HRESULT hres;

	hres = CreateDDSTextureFromFile(dev, filename, &m_texture, &m_textureView, size, nullptr);
	if (FAILED(hres)) return false;
	return result;
}

