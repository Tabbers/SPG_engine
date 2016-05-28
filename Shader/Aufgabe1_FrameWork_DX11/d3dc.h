#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_
#include <d3d11.h>
#include <DirectXMath.h>
#include "globaldefinitions.h"
#include "d3drendertotexture.h"

using namespace DirectX;

class D3Dc
{
public:
	D3Dc();
	D3Dc(const D3Dc&);
	~D3Dc();

	bool Init(int,int,bool, HWND, bool);
	void BeginScene(XMFLOAT4);
	void EndScene();

	inline ID3D11Device* D3Dc::GetDevice() { return m_device; };
	inline ID3D11DeviceContext* D3Dc::GetDeviceContext() { return m_devCon; };

	inline int GetMSAA() { return m_msaaIndex; };
	inline void SetMSAA(int index) { m_msaaIndex = index; };

	inline MSAASettings GetMSAASettings() { return m_msaa; };
	inline ID3D11Texture2D* GetBackBuffer() { return m_backBufferPtr; };
	inline IDXGISwapChain* GetSwapchain() {return m_swapChain; };

	inline void D3Dc::GetWorldMatrix(XMMATRIX& worldMatrixExt) 
	{ 
		worldMatrixExt = m_worldMatrix;
		return;
	};

	inline void D3Dc::GetVideoCard(char * cardName, int& memory) {
		strcpy_s(cardName, 129, m_vCardDesc);
		memory = m_videoCardMemory;
		return;
	};

	inline void D3Dc::getScreenDimensions(int & screenWidth, int &screenHeight)
	{
		screenWidth = this->m_screenWidth;
		screenHeight = this->m_screenHeight;
	};
	inline void D3Dc::SetBackBufferRenderTarget() {	m_devCon->OMSetRenderTargets(1, &m_targetView, m_depthStencilView); };

	inline void EnableFrontCulling()
	{
		m_devCon->RSSetState(m_rasterStateFrontCulling);
	}
	inline void EnableBackCulling()
	{
		m_devCon->RSSetState(m_rasterStateBackCulling);
	}
	inline void EnableWireframe()
	{
		m_devCon->RSSetState(m_rasterStateWireFrame);
	}
	inline void EnableAlphaBlending() 
	{
		FLOAT blend[4] = { 1.0f,1.0f,1.0f,0.0f };
		m_devCon->OMSetBlendState(m_blendstateAlphaEnabled, blend , 0xFFFFFFFF); 
		m_devCon->OMSetDepthStencilState(m_depthStencilParticleState, 1);
	};

	inline void DisableAlphaBlending()
	{
		FLOAT blend[4] = { 0.0f,0.0f,0.0f,0.0f };
		m_devCon->OMSetBlendState(NULL, blend, 0xFFFFFFFF);
		m_devCon->OMSetDepthStencilState(m_depthStencilState, 1);

	};

	bool g_Wireframe = false;
private:
	std::vector<IDXGIAdapter*> EnumerateAdapters(void);

	bool m_Wireframe = false;
	bool m_vsync_on;
	int m_videoCardMemory;
	char m_vCardDesc[128];
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_devCon;
	IDXGISwapChain* m_swapChain;
	ID3D11RenderTargetView* m_targetView;
	ID3D11Texture2D* m_depthBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11DepthStencilState* m_depthStencilParticleState;
	ID3D11RasterizerState* m_rasterStateBackCulling;
	ID3D11RasterizerState* m_rasterStateFrontCulling;
	ID3D11RasterizerState* m_rasterStateWireFrame;
	ID3D11Texture2D* m_backBufferPtr;
	ID3D11BlendState* m_blendstateAlphaEnabled;
	ID3D11BlendState* m_blendstateAlphaDisabled;
	//Matrix housing world positions
	XMMATRIX m_worldMatrix;
	int m_screenWidth, m_screenHeight;
	//MSAA settings
	MSAASettings m_msaa;
	int m_msaaIndex=0;
};
#endif

