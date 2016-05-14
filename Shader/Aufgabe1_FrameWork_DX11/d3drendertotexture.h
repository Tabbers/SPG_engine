#pragma once
#ifndef _D3DRENDERTOTEXTURE_H_
#define _D3DRENDERTOTEXTURE_H_

#include <d3d11.h>
#include <directxmath.h>

class D3DRenderToTexture
{
public:
	D3DRenderToTexture();
	~D3DRenderToTexture();
	bool Init(ID3D11Device*,int,int,int,int, DXGI_FORMAT format);

	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*,DirectX::XMFLOAT4);
	//getter
	inline ID3D11ShaderResourceView* GetShaderRessourceView() { return m_shaderResourceView;};
	inline ID3D11Texture2D* GetTexture() { return m_renderTargetTexture; };
	inline void GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix) { projectionMatrix = m_projectionMatrix; };
	inline void GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix) { orthoMatrix = m_orthoMatrix; };
private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_orthoMatrix;
	
};
#endif

